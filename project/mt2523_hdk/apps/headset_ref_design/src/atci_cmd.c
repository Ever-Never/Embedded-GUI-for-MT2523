/* Copyright Statement:
 *
 * (C) 2005-2017  MediaTek Inc. All rights reserved.
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

/* Includes ------------------------------------------------------------------*/
// For Register AT command handler
#include "atci.h"

// System head file
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "syslog.h"

#include "task_def.h"

#include "ff.h"
#include "bt_sink_srv.h"
#include "voice_prompt_mix.h"
#include "bt_sink_app_main.h"

#ifdef MTK_PROMPT_SOUND_ENABLE
#include "audio_mixer.h"
#include "prompt_control.h"
#include "hal_feature_config.h"
#endif

//log_create_module(atcmd, PRINT_LEVEL_INFO);
#define LOGE(fmt,arg...)   LOG_E(atcmd, "ATCI project: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atcmd, "ATCI project: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atcmd, "ATCI project: "fmt,##arg)

/*
 * sample code
*/

/*--- Function ---*/
atci_status_t atci_cmd_hdlr_testcmd(atci_parse_cmd_param_t *parse_cmd);


#ifdef MTK_PROMPT_SOUND_ENABLE
static atci_status_t atci_cmd_hdlr_voice_prompt_mix_cmd(atci_parse_cmd_param_t *parse_cmd);
#endif


/*---  Variant ---*/
atci_cmd_hdlr_item_t item_table[] = {
    {"AT+TESTCMD",      atci_cmd_hdlr_testcmd,      0, 0},

#ifdef MTK_PROMPT_SOUND_ENABLE
    {"AT+MIXTONE",      atci_cmd_hdlr_voice_prompt_mix_cmd,  0, 0},
#endif
};


void atci_example_init()
{
    atci_status_t ret = ATCI_STATUS_REGISTRATION_FAILURE;

    LOGW("atci_example_init\r\n");

    // --------------------------------------------------------- //
    // ------- Test Scenario: register AT handler in CM4 ------- //
    // --------------------------------------------------------- //
    ret = atci_register_handler(item_table, sizeof(item_table) / sizeof(atci_cmd_hdlr_item_t));
    if (ret == ATCI_STATUS_OK) {
        LOGW("at_example_init register success\r\n");
    } else {
        LOGW("at_example_init register fail\r\n");
    }

#ifdef MTK_PROMPT_SOUND_ENABLE
#if defined(__AFE_HS_DC_CALIBRATION__)
    hal_audio_set_stream_out_device(HAL_AUDIO_DEVICE_HANDSET);
#else
    hal_audio_set_stream_out_device(HAL_AUDIO_DEVICE_HEADSET);
#endif
#endif
}

// AT command handler
atci_status_t atci_cmd_hdlr_testcmd(atci_parse_cmd_param_t *parse_cmd)
{
    static int test_param1 = 0;
    atci_response_t response = {{0}};
    char *param = NULL;
    int  param1_val = -1;

    LOGW("atci_cmd_hdlr_testcmd\n");

    response.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+TESTCMD=?
            LOGW("AT Test OK.\n");
            strcpy((char *)response.response_buf, "+TESTCMD:(0,1)\r\nOK\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+TESTCMD?
            LOGW("AT Read done.\n");
            sprintf((char *)response.response_buf,"+TESTCMD:%d\r\n", test_param1);
            response.response_len = strlen((char *)response.response_buf);
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&response);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+TESTCMD
            LOGW("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)response.response_buf, "+TESTCMD:HELLO.OK\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+TESTCMD=<p1>  the handler need to parse the parameters
            LOGW("AT Executing...\r\n");
            //parsing the parameter
            param = strtok(parse_cmd->string_ptr, ",\n\r");
	        param = strtok(parse_cmd->string_ptr, "AT+TESTCMD=");
            param1_val = atoi(param);

	    if (param != NULL && (param1_val == 0 || param1_val == 1)){

                // valid parameter, update the data and return "OK"
                response.response_len = 0;
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK; // ATCI will help append "OK" at the end of resonse buffer
            } else {
                // invalide parameter, return "ERROR"
                response.response_len = 0;
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of resonse buffer
            };
            atci_send_response(&response);
			param = NULL;
            break;

        default :
            strcpy((char *)response.response_buf, "ERROR\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
    return ATCI_STATUS_OK;
}

#ifdef MTK_PROMPT_SOUND_ENABLE
static uint8_t voice_prompt_mix_find_voice_index(char *str)
{
    uint8_t i;
    char *voice_idx[] = {"8K","11K","12K","16K","22K","24K","32K","44.1K","48K"};

    for(i=0; i<sizeof(voice_idx)/sizeof(voice_idx[0]); i++)
    {
        if (0 == strncmp(str, voice_idx[i], strlen(voice_idx[i])))
            break;
    }
    return (i<sizeof(voice_idx)/sizeof(voice_idx[0])) ? i : 0xFF;
}

typedef struct {
    hal_audio_sampling_rate_t       sampling_rate_enum;
    hal_audio_channel_number_t      channel_number_enum;
    audio_mixer_role_t              role;
    audio_mixer_track_id_t          track_id;
} test_track_info_t;

static test_track_info_t KH_Track1, KH_Track2;

// If sampling rate is 8kHz stereo, it is 500 Hz tone 
static const uint8_t Test_PCM1[64] =
{
  0xFC, 0xFF, 0x27, 0x20, 0x4B, 0x0C, 0xB5, 0x1D,
  0xBA, 0x16, 0xBD, 0x16, 0xB2, 0x1D, 0x51, 0x0C,
  0x27, 0x20, 0x03, 0x00, 0xB6, 0x1D, 0xB5, 0xF3,
  0xBE, 0x16, 0x46, 0xE9, 0x51, 0x0C, 0x4D, 0xE2,
  0x03, 0x00, 0xD9, 0xDF, 0xB5, 0xF3, 0x4A, 0xE2,
  0x46, 0xE9, 0x42, 0xE9, 0x4D, 0xE2, 0xB0, 0xF3,
  0xDA, 0xDF, 0xFD, 0xFF, 0x4A, 0xE2, 0x4B, 0x0C,
  0x42, 0xE9, 0xBA, 0x16, 0xB0, 0xF3, 0xB3, 0x1D
};

// If sampling rate is 8kHz mono, it is 1000 Hz tone
static const uint8_t Test_PCM2[16] =
{
  0xFD, 0xFF, 0xBA, 0x16, 0x27, 0x20, 0xBF, 0x16,
  0x02, 0x00, 0x46, 0xE9, 0xD9, 0xDF, 0x42, 0xE9
};

static void Track_Callback1(hal_audio_event_t event, void *data)
{
    test_track_info_t *pTrackHandle;
    uint32_t dst_cnt  = 0;
    uint32_t i;
    
    pTrackHandle = (test_track_info_t *)data;

    audio_mixer_query_free_count(pTrackHandle->track_id, &dst_cnt);

    i = dst_cnt/64;
    for ( ; i>0; i--) {
        audio_mixer_write_data(pTrackHandle->track_id, Test_PCM1, 64);
    }
}

static void Track_Callback2(hal_audio_event_t event, void *data)
{
    test_track_info_t *pTrackHandle;
    uint32_t dst_cnt  = 0;
    uint32_t i;
    
    pTrackHandle = (test_track_info_t *)data;

    audio_mixer_query_free_count(pTrackHandle->track_id, &dst_cnt);

    i = dst_cnt/16;
    for ( ; i>0; i--) {
        audio_mixer_write_data(pTrackHandle->track_id, Test_PCM2, 16);
    }
}

static atci_status_t atci_cmd_hdlr_voice_prompt_mix_cmd(atci_parse_cmd_param_t *parse_cmd)
{
    voice_prompt_mix_tone_info_t tone_info;
    atci_response_t resonse = {{0}};
    resonse.response_flag = 0; // Command Execute Finish.
    bt_sink_app_device_role_t role;
    
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: // rec: AT+MIXTONE=<p1>
        	
        	
          role = bt_sink_app_get_device_role();
          if (role != BT_SINK_APP_DEVICE_SLAVE)        	
        	{
        		
            LOGW("(voice_prompt_mix)AT Executing...%s\n", parse_cmd->string_ptr);
            //parsing the parameter
            if (0 == memcmp(parse_cmd->string_ptr+3, "MIXTONE", 7))
            {
                if (0 == memcmp(parse_cmd->string_ptr+11, "SETMIXERGAIN", 12))
                {
                    //STOP TONE: AT+MIXTONE=SETMIXERGAIN_[MAIN_GAIN]_[SIDE_GAIN]
                    uint16_t main_gain, side_gain;
                    char *endp = "_";

                    main_gain = strtol(parse_cmd->string_ptr+24, &endp, 16);
                    side_gain = strtol(parse_cmd->string_ptr+29, &endp, 16);
                    prompt_control_set_mixer_gain(main_gain, side_gain);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else if (0 == memcmp(parse_cmd->string_ptr+11, "STOP", 4))
                {
                    //STOP TONE: AT+MIXTONE=STOP
                    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_MIXER_TONE_STOP, NULL);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else if (0 == memcmp(parse_cmd->string_ptr+11, "PLAY_WAV_IDX_", 13))
                {
                    //PLAY WAV TONE: AT+MIXTONE=PLAY_WAV_IDX_[SAMPLE_RATE]
                    tone_info.tone_type = VPC_WAV;
                    tone_info.tone_idx = voice_prompt_mix_find_voice_index(parse_cmd->string_ptr+24);
                    if (tone_info.tone_idx == 0xFF) {
                        resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    }
                    else {
                        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_MIXER_TONE_PLAY, (bt_sink_srv_event_param_t *)&tone_info);
                        resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    }
                }
                else if (0 == memcmp(parse_cmd->string_ptr+11, "PLAY_MP3_IDX_", 13))
                {
                    //PLAY WAV TONE: AT+MIXTONE=PLAY_MP3_IDX_[SAMPLE_RATE]
                    tone_info.tone_type = VPC_MP3;
                    tone_info.tone_idx = voice_prompt_mix_find_voice_index(parse_cmd->string_ptr+24);
                    if (tone_info.tone_idx == 0xFF) {
                        resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    }
                    else {
                        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_MIXER_TONE_PLAY, (bt_sink_srv_event_param_t *)&tone_info);
                        resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    }
                }
                else if (0 == memcmp(parse_cmd->string_ptr+11, "A1", 2))
                {
                    KH_Track1.sampling_rate_enum = HAL_AUDIO_SAMPLING_RATE_8KHZ;
                    KH_Track1.channel_number_enum = HAL_AUDIO_STEREO;
                    KH_Track1.role = AUDIO_MIXER_TRACK_ROLE_MAIN;
    
                    audio_mixer_set_volume(0x4000, 0x0800);

                    KH_Track1.track_id = audio_mixer_get_track_id(KH_Track1.sampling_rate_enum,
                                                   KH_Track1.channel_number_enum,
                                                   Track_Callback1,
                                                   &KH_Track1,
                                                   KH_Track1.role);

                    LOGW("KH mix ID =%d\n", KH_Track1.track_id);

                    if (KH_Track1.track_id < 0) {
                        resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    } else {
                        Track_Callback1(HAL_AUDIO_EVENT_DATA_REQUEST, &KH_Track1);

                        audio_mixer_start_track(KH_Track1.track_id);

                        resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    }
                }
                else if (0 == memcmp(parse_cmd->string_ptr+11, "B1", 2))
                {
                    audio_mixer_stop_track(KH_Track1.track_id);
                    audio_mixer_free_track_id(KH_Track1.track_id);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else if (0 == memcmp(parse_cmd->string_ptr+11, "A2", 2))
                {
                    KH_Track2.sampling_rate_enum = HAL_AUDIO_SAMPLING_RATE_8KHZ;
                    KH_Track2.channel_number_enum = HAL_AUDIO_MONO;
                    KH_Track2.role = AUDIO_MIXER_TRACK_ROLE_SIDE;
    
                    audio_mixer_set_volume(0x4000, 0x0800);

                    KH_Track2.track_id = audio_mixer_get_track_id(KH_Track2.sampling_rate_enum,
                                                   KH_Track2.channel_number_enum,
                                                   Track_Callback2,
                                                   &KH_Track2,
                                                   KH_Track2.role);

                    LOGW("KH mix ID =%d\n", KH_Track2.track_id);

                    if (KH_Track2.track_id < 0) {
                        resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    } else {
                        Track_Callback2(HAL_AUDIO_EVENT_DATA_REQUEST, &KH_Track2);

                        audio_mixer_start_track(KH_Track2.track_id);

                        resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    }
                }
                else if (0 == memcmp(parse_cmd->string_ptr+11, "B2", 2))
                {
                    audio_mixer_stop_track(KH_Track2.track_id);
                    audio_mixer_free_track_id(KH_Track2.track_id);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else if (0 == memcmp(parse_cmd->string_ptr+11, "D0", 2))
                {
#if defined(__AFE_HS_DC_CALIBRATION__)
                    hal_audio_set_stream_out_device(HAL_AUDIO_DEVICE_HANDSET);
#else
                    hal_audio_set_stream_out_device(HAL_AUDIO_DEVICE_HEADSET);
#endif
                }
                else
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            else
            {
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            resonse.response_len = 0;
            atci_send_response(&resonse);
            break;
          }
          else
          {
            strcpy((char *)resonse.response_buf, "SLAVE_MODE\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;          	    	
          }


        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }
    return ATCI_STATUS_OK;
}
#endif
