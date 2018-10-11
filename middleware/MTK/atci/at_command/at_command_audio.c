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
#include <string.h>
#include <stdio.h>

#include "hal_audio.h"

#if defined(HAL_AUDIO_MODULE_ENABLED)

#include "hal_audio_test.h"
#include "hal_audio_enhancement.h"

#ifdef MTK_EXTERNAL_DSP_ENABLE
#include "external_dsp_application.h"
#include "external_dsp_driver.h"

#if defined (MTK_NDVC_ENABLE)
extern uint16_t spe_ndvc_uplink_noise_index_map(uint16_t db);
bool ndvc_at_test = false;
#endif /*MTK_NDVC_ENABLE*/

#endif /*MTK_EXTERNAL_DSP_ENABLE*/

#if defined(MTK_BT_AWS_ENABLE)
#include "hal_audio_internal_service.h"
#endif  /* defined(MTK_BT_AWS_ENABLE)) */

#if defined(MTK_BT_HFP_CODEC_DEBUG)
#include "bt_hfp_codec_internal.h"
typedef enum {
    AT_AUDIO_HFP_SAVE_OR_PRINT_METHOD_WRITE_TO_FILE = 0,
    AT_AUDIO_HFP_SAVE_OR_PRINT_METHOD_PRINT_TO_USB_DEBUG_PORT = 1,
} at_audio_hfp_save_or_print_method_t;
#endif

// AT command handler
atci_status_t atci_cmd_hdlr_audio(atci_parse_cmd_param_t *parse_cmd)
{
#if defined(__GNUC__)
    atci_response_t response = {{0}};
    log_hal_info("atci_cmd_hdlr_audio \r\n");

    response.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+ECMP=?
            strcpy((char * restrict)response.response_buf, "+EAUDIO =<op>[,<param>]\r\nOK\r\n");
            response.response_len = strlen((const char *)response.response_buf);
            atci_send_response(&response);
            break;
        case ATCI_CMD_MODE_EXECUTION: // rec: AT+EAUDIO=<op>  the handler need to parse the parameters
            if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DUMMY") != NULL) {
                log_hal_info("This is a summy command in order to have \"if\" description for if loop\r\n");
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
#if defined(HAL_AUDIO_TEST_ENABLE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Audio_Play_HEADSET") != NULL) {
                audio_test_set_output_device(HAL_AUDIO_DEVICE_HEADSET);
                audio_test_set_audio_tone(true);
                uint8_t result = audio_test_play_audio_1k_tone();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Audio_Play_HANDSET") != NULL) {
                audio_test_set_output_device(HAL_AUDIO_DEVICE_HANDS_FREE_MONO);
                audio_test_set_audio_tone(true);
                uint8_t result = audio_test_play_audio_1k_tone();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Voice_Play_HEADSET") != NULL) {
                audio_test_set_output_device(HAL_AUDIO_DEVICE_HEADSET);
                audio_test_set_audio_tone(false);
                uint8_t result = audio_test_play_voice_1k_tone();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Play_Stop") != NULL) {
                uint8_t result = audio_test_stop_1k_tone();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Dump_SPE_COM_Param") != NULL) {
                spe_dump_common_param();
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Dump_SPE_MOD_Param") != NULL) {
                spe_dump_mode_param();
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(&response);
            }
#if defined(HAL_AUDIO_SDFATFS_ENABLE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Record_MMIC_ACC,") != NULL) {
                uint8_t result = 0;
                char* config_s = NULL;
                unsigned int config = 0;
                config_s = strchr(parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%x", &config);
                audio_test_set_input_device(HAL_AUDIO_DEVICE_MAIN_MIC);
                audio_test_set_audio_tone(false);
                audio_test_switch_mic_type(0); //acc
                if(config == 0) {
                    result = audio_test_pcm2way_record();
                } else {
                    result = audio_test_pcm2way_wb_record();
                }
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Record_MMIC_DCC,") != NULL) {
                uint8_t result = 0;
                char* config_s = NULL;
                unsigned int config = 0;
                config_s = strchr(parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%x", &config);
                audio_test_set_input_device(HAL_AUDIO_DEVICE_MAIN_MIC);
                audio_test_set_audio_tone(false);
                audio_test_switch_mic_type(1); //dcc
                if(config == 0) {
                    result = audio_test_pcm2way_record();
                } else {
                    result = audio_test_pcm2way_wb_record();
                }
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Record_DMIC") != NULL) {
                audio_test_set_input_device(HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC);
                audio_test_set_audio_tone(false);
                uint8_t result = audio_test_pcm2way_record();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Record_Test_Off") != NULL) {
                uint8_t result = audio_test_pcm2way_stop_1k_tone();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Audio_Play_SD") != NULL) {
                uint8_t result = audio_test_play_audio_sd();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Audio_Stop_SD") != NULL) {
                uint8_t result = audio_test_stop_audio_sd();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
#endif /* HAL_AUDIO_SDFATFS_ENABLE */
#ifdef MTK_BUILD_SMT_LOAD
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Ext_LPK_HEADSET_HMIC") != NULL) {
                audio_test_set_output_device(HAL_AUDIO_DEVICE_HEADSET);
                audio_test_set_input_device(HAL_AUDIO_DEVICE_HEADSET_MIC);
                audio_test_set_audio_tone(false);
                uint8_t result = audio_test_external_loopback_test();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Ext_LPK_SPK_MMIC") != NULL) {
                audio_test_set_input_device(HAL_AUDIO_DEVICE_MAIN_MIC);
                audio_test_set_output_device(HAL_AUDIO_DEVICE_HANDS_FREE_MONO);
                audio_test_set_audio_tone(false);
                uint8_t result = audio_test_external_loopback_test();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Ext_LPK_SPK_DMIC") != NULL) {
                audio_test_set_input_device(HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC);
                audio_test_set_output_device(HAL_AUDIO_DEVICE_HANDS_FREE_MONO);
                audio_test_set_audio_tone(false);
                uint8_t result = audio_test_external_loopback_test();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=SMT_RESULT") != NULL) {
                uint8_t result = audio_test_detect_1k_tone_result();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
#ifdef HAL_ACCDET_MODULE_ENABLED
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ACCDET_TEST") != NULL) {
                uint8_t result = audio_test_detect_earphone();
                snprintf((char * restrict)response.response_buf, sizeof(response.response_buf), "+EAUDIO:%d\r\n", result);
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ACCDET_REG") != NULL) {
                //callback
                register_accdet_callback();
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
#endif /* HAL_ACCDET_MODULE_ENABLED */
#endif /* MTK_BUILD_SMT_LOAD */
#if defined(HAL_AUDIO_SLT_ENABLE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=SLT_START") != NULL) {
                uint8_t result = audio_slt_test();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=SLT_RESULT") != NULL) {
                audio_test_stop_1k_tone();
                uint8_t result = audio_test_detect_1k_tone_result();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
#endif
#ifdef MTK_EXTERNAL_DSP_ENABLE
#if defined (MTK_NDVC_ENABLE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=NDVC,") != NULL) {
                char* noise_ptr = NULL;
                uint16_t noise_level =0, noise_idx=0;
                noise_ptr = strchr(parse_cmd->string_ptr, ',');
                noise_ptr++;
                noise_level = atoi(noise_ptr);
                ndvc_at_test = true;
                noise_idx = spe_ndvc_uplink_noise_index_map(noise_level);
                *DSP_SPH_SWNDVC_POWER_INDEX = noise_idx;
                log_hal_info("\r\n[AT]NDVC Test noise_dB=%d, index=%d \r\n", noise_level, noise_idx);
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=TESTATNDVCCOFF") != NULL) {
                ndvc_at_test = false;
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(&response);
            }
#endif /*MTK_NDVC_ENABLE*/
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DSP_PWR_ON_DL") != NULL) {
                uint8_t result = external_dsp_activate(true);
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DSP_PWR_OFF") != NULL) {
                uint8_t result = external_dsp_activate(false);
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DSP_SET_MIPS,") != NULL) {
                char *token = strtok(parse_cmd->string_ptr, ","); //s
                uint8_t cnt = 0;
                uint32_t mips = 0;
                while( token != NULL) {
                    if(cnt == 1) {
                        sscanf(token, "0x%x", (unsigned int *)&mips);
                        log_hal_info("mips=%x(%s)\r\n", mips, token);
                    } else {
                        log_hal_info("parse failed:%s!\r\n", token);
                    }
                    cnt ++;
                    token = strtok(NULL, ",");
                }
                uint8_t result = external_dsp_set_clock_rate(mips);
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DSP_GET_MIPS") != NULL) {
                uint32_t mips = 0;
                uint8_t result = external_dsp_get_clock_rate(&mips);
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DSP_SPE,") != NULL) {
                char* config_s = NULL;
                unsigned int config = 0;
                config_s = strchr(parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%x", &config);
                log_hal_info("config=%x\r\n", config);
                external_dsp_spi_init();
                external_dsp_set_output_source(config);
                external_dsp_configure_data_path(config);
                external_dsp_spi_deinit();
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
            /* SMT item for MTK automotive product */
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=UL2DL,") != NULL) {
                char *token = strtok(parse_cmd->string_ptr, ","); //s
                uint8_t cnt = 0, result = 0;
                bool    mix_tone = false;
                uint32_t delay_ms = 0;
                while( token != NULL) {
                    if(cnt == 1) {
                        if (strncmp(token, "true", strlen("true")) == 0) {
                            mix_tone = true;
                        } else {
                            mix_tone = false;
                        }
                        log_hal_info("mix_tone=%x(%s)\r\n", mix_tone, token);
                    } else if(cnt == 2) {
                        sscanf(token, "%x", (unsigned int *)&delay_ms);
                        log_hal_info("delay_ms=%x(%s)\r\n", delay_ms, token);
                    } else {
                        log_hal_info("parse failed:%s!\r\n", token);
                    }
                    cnt ++;
                    token = strtok(NULL, ",");
                }
                result = audio_test_loopback_ul2dl_mixer(mix_tone, delay_ms);
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DSP_RD_DRAM,") != NULL) {
                char *token = strtok(parse_cmd->string_ptr, ",");
                uint8_t cnt = 0, result = 0;
                uint32_t address = 0;
                uint16_t value = 0;
                while( token != NULL) {
                    if(cnt == 1) {
                        sscanf(token, "0x%x", (unsigned int *)&address);
                        log_hal_info("address=%x(%s)\r\n", address, token);
                    } else {
                        log_hal_info("parse failed:%s!\r\n", token);
                    }
                    cnt ++;
                    token = strtok(NULL, ",");
                }
                result = external_dsp_read_dram_word(address, &value);
                sprintf((char *)response.response_buf, "0x%x", value);
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DSP_WR_DRAM,") != NULL) {
                char *token = strtok(parse_cmd->string_ptr, ",");
                uint8_t cnt = 0, result = 0;
                uint32_t address = 0;
                uint32_t value = 0;
                while( token != NULL) {
                    if(cnt == 1) {
                        sscanf(token, "%x", (unsigned int *)&address);
                        log_hal_info("address=%x(%s)\r\n", address, token);
                    #if 1
                    } else if(cnt == 2) {
                        sscanf(token, "%x", (unsigned int *)&value);
                        log_hal_info("value=%x(%s)\r\n", value, token);
                    #endif
                    }
                    cnt ++;
                    token = strtok(NULL, ",");
                }
                log_hal_info("address=%x, value=%x\r\n", address, value);
                result = external_dsp_write_dram_word(address, (uint16_t)value);
                sprintf((char *)response.response_buf, "0x%x", (unsigned int)value);
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
#if defined(HAL_AUDIO_SLT_ENABLE) || defined(MTK_BUILD_SMT_LOAD)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DSP_SMT_1") != NULL) {
                log_hal_info("SMT Test 1. Dual dmic bypass test\r\n");
                uint8_t result = audio_test_dual_dmic_bypass();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DSP_SMT_2") != NULL) {
                log_hal_info("SMT Test 2-1. Download external dsp firmware\r\n");
                uint8_t result = audio_test_download_external_dsp();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DSP_SMT_3") != NULL) {
                log_hal_info("SMT Test 2-2. I2S external loopback\r\n");
                uint8_t result = audio_test_i2s_external_loopback();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DSP_SLT") != NULL) {
                log_hal_info("[Audio Ext-DSP]SLT Test\r\n");
                uint8_t result = audio_external_dsp_slt_test();
                if(result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(&response);
            }
#endif /* defined(HAL_AUDIO_SLT_ENABLE) || defined(MTK_BUILD_SMT_LOAD) */
#endif /* #ifdef MTK_EXTERNAL_DSP_ENABLE */

#if defined(MTK_BT_AWS_ENABLE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AWS,") != NULL) {
                char *string_pointer = (char *)parse_cmd->string_ptr;
                int32_t result;
                if (strstr(string_pointer, "AWS,STATUS") != NULL) {
                    aws_clock_skew_status_t status;
                    status = audio_service_aws_get_clock_skew_status();
                    if (status == AWS_CLOCK_SKEW_STATUS_IDLE) {
                        log_hal_info("[AWS AT CMD]AWS_CLOCK_SKEW_STATUS_IDLE \n");
                        result = HAL_AUDIO_AWS_NORMAL;
                    } else if (status == AWS_CLOCK_SKEW_STATUS_BUSY) {
                        log_hal_info("[AWS AT CMD]AWS_CLOCK_SKEW_STATUS_BUSY \n");
                        result = HAL_AUDIO_AWS_NORMAL;
                    } else {
                        result = HAL_AUDIO_AWS_ERROR;
                    }
                } else if (strstr(string_pointer, "AWS,SKEW,") != NULL) {
                    char *p_string = strstr(string_pointer, "AWS,SKEW,");
                    int32_t val = 0;
                    p_string += strlen("AWS,SKEW,");
                    sscanf(p_string, "%d", (int *)&val);
                    result = audio_service_aws_set_clock_skew_compensation_value(val);
                } else if (strstr(string_pointer, "AWS,CNT") != NULL) {
                    uint32_t value;
                    hal_gpio_init(HAL_GPIO_12);
                    hal_pinmux_set_function(HAL_GPIO_12, 0);
                    hal_gpio_set_direction(HAL_GPIO_12, HAL_GPIO_DIRECTION_OUTPUT);
                    hal_gpio_set_output(HAL_GPIO_12, HAL_GPIO_DATA_LOW);
                    hal_gpio_set_output(HAL_GPIO_12, HAL_GPIO_DATA_HIGH);
                    hal_gpio_set_output(HAL_GPIO_12, HAL_GPIO_DATA_LOW);
                    value = audio_service_aws_get_accumulated_sample_count();
                    log_hal_info("[AWS AT CMD]AWS Accumulate count: %d \n", value);
                    result = HAL_AUDIO_AWS_NORMAL;
                } else if (strstr(string_pointer, "AWS,FILLSIL,AAC,") != NULL) {
                    char *p_string = strstr(string_pointer, "AWS,FILLSIL,AAC,");
                    uint32_t *sil_frm_cnt;
                    uint32_t temp = 100;
                    uint32_t *byte_cnt = &temp;
                    log_hal_info("byte_cnt: %d", *byte_cnt);
                    uint32_t val;
                    p_string += strlen("AWS,FILLSIL,AAC,");
                    sscanf(p_string, "%d", (unsigned int*)&val);
                    sil_frm_cnt = &val;
                    log_hal_info("sil_frame_count: %d", *sil_frm_cnt);
                    uint8_t temp_buffer[100];
                    memset(temp_buffer, 0, 100 * sizeof(uint8_t));
                    log_hal_info("[AWS AT CMD]before fill silence: buffer space byte count: %d, sil frm user want to fill: %d", *byte_cnt, *sil_frm_cnt);
                    result = audio_service_aws_fill_silence_frame(temp_buffer, byte_cnt, AWS_CODEC_TYPE_AAC_FORMAT, sil_frm_cnt);
                    log_hal_info("[AWS AT CMD]after fill silence: buffer space byte count: %d, sil frm user fill: %d", *byte_cnt, *sil_frm_cnt);
                    log_hal_info("[AWS AT CMD]buffer print: ");
                    for (int i = 0; i < 100; i++) {
                        log_hal_info("0x%x ", temp_buffer[i]);
                    }
                } else {
                    log_hal_info("[AWS AT CMD]Invalid AWS AT command : %s\r\n", string_pointer);
                    result = HAL_AUDIO_AWS_ERROR;
                }
                response.response_flag = result == HAL_AUDIO_AWS_NORMAL ? ATCI_RESPONSE_FLAG_APPEND_OK : ATCI_RESPONSE_FLAG_APPEND_ERROR;
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
#endif  /* defined(MTK_BT_AWS_ENABLE) */
#endif /* defined(HAL_AUDIO_TEST_ENABLE) */

#if defined(MTK_BT_HFP_CODEC_DEBUG)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DEBUG,") != NULL) {   // for debug use
                log_hal_info("DEBUG AT command : %s\r\n", parse_cmd->string_ptr);

                char *string_pointer = (char *)parse_cmd->string_ptr;
                int32_t result;
                if (strstr(string_pointer, "DEBUG,HFP,") != NULL) { // debug hfp
                    if (strstr(string_pointer, "DEBUG,HFP,DL_ESCO,") != NULL) { // debug hfp DL_ESCO
                        char *p_string = strstr(string_pointer, "DEBUG,HFP,DL_ESCO,");
                        p_string += strlen("DEBUG,HFP,DL_ESCO,");

                        uint32_t save_or_print_method = 0;
                        sscanf(p_string, "%d", (int *)&save_or_print_method);
                        if (save_or_print_method == AT_AUDIO_HFP_SAVE_OR_PRINT_METHOD_WRITE_TO_FILE) {
                            bt_hfp_codec_debug_open(BT_HFP_CODEC_DEBUG_FLAG_DOWNLINK_ESCO_RAW_DATA, BT_HFP_CODEC_DEBUG_SAVE_OR_PRINT_METHOD_SAVE_TO_SDCARD);
                            result = ATCI_RESPONSE_FLAG_APPEND_OK;
                        } else {
                            log_hal_info("DEBUG,HFP,DL_ESCO: invalid save_or_print_method(%d)\r\n", save_or_print_method);
                            result = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                        }
                    } else if (strstr(string_pointer, "DEBUG,HFP,DL_STREAM_OUT_PCM,") != NULL) {
                        char *p_string = strstr(string_pointer, "DEBUG,HFP,DL_STREAM_OUT_PCM,");
                        p_string += strlen("DEBUG,HFP,DL_STREAM_OUT_PCM,");

                        uint32_t save_or_print_method = 0;
                        sscanf(p_string, "%d", (int *)&save_or_print_method);
                        if (save_or_print_method == AT_AUDIO_HFP_SAVE_OR_PRINT_METHOD_WRITE_TO_FILE) {
                            bt_hfp_codec_debug_open(BT_HFP_CODEC_DEBUG_FLAG_DOWNLINK_STREAM_OUT_PCM, BT_HFP_CODEC_DEBUG_SAVE_OR_PRINT_METHOD_SAVE_TO_SDCARD);
                            result = ATCI_RESPONSE_FLAG_APPEND_OK;
                        } else {
                            log_hal_info("DEBUG,HFP,DL_STREAM_OUT_PCM: invalid save_or_print_method(%d)\r\n", save_or_print_method);
                            result = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                        }
                    } else if (strstr(string_pointer, "DEBUG,HFP,VM,") != NULL) {
                        char *p_string = strstr(string_pointer, "DEBUG,HFP,VM,");
                        p_string += strlen("DEBUG,HFP,VM,");

                        uint32_t save_or_print_method = 0;
                        sscanf(p_string, "%d", (int *)&save_or_print_method);
                        if (save_or_print_method == AT_AUDIO_HFP_SAVE_OR_PRINT_METHOD_WRITE_TO_FILE) {
                            bt_hfp_codec_debug_open(BT_HFP_CODEC_DEBUG_FLAG_VM_LOG, BT_HFP_CODEC_DEBUG_SAVE_OR_PRINT_METHOD_SAVE_TO_SDCARD);
                            result = ATCI_RESPONSE_FLAG_APPEND_OK;
                        } else if (save_or_print_method == AT_AUDIO_HFP_SAVE_OR_PRINT_METHOD_PRINT_TO_USB_DEBUG_PORT) {
                            bt_hfp_codec_debug_open(BT_HFP_CODEC_DEBUG_FLAG_VM_LOG, BT_HFP_CODEC_DEBUG_SAVE_OR_PRINT_METHOD_PRINT_TO_USB_DEBUG_PORT);
                            result = ATCI_RESPONSE_FLAG_APPEND_OK;
                        } else {
                            log_hal_info("DEBUG,HFP,VM: invalid save_or_print_method(%d)\r\n", save_or_print_method);
                            result = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                        }
                    } else if (strstr(string_pointer, "DEBUG,HFP,STOP") != NULL) {
                        bt_hfp_codec_debug_close();
                        result = ATCI_RESPONSE_FLAG_APPEND_OK;
                    } else {
                        log_hal_info("Invalid DEBUG,HFP AT command : %s\r\n", string_pointer);
                        result = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    }
                } else {
                    log_hal_info("Invalid DEBUG AT command : %s\r\n", string_pointer);
                    result = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                response.response_flag = result;
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
#endif  /* defined(BT_HFP_CODEC_DEBUG) */
            else
            {
                /* invalid AT command */
                log_hal_info("atci_cmd_hdlr_audio: command not exist \r\n");
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                response.response_len = strlen((const char *)response.response_buf);
                atci_send_response(&response);
            }
            break;

        default :
            /* others are invalid command format */
            strcpy((char * restrict)response.response_buf, "ERROR\r\n");
            response.response_len = strlen((const char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
#endif /*defined(__GNUC__)*/
    return ATCI_STATUS_OK;
}

#endif /* defined(HAL_AUDIO_MODULE_ENABLED) */
