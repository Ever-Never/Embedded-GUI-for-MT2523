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

#include "bt_sink_srv_audio_tunning.h"
#ifdef __BT_SINK_AUDIO_TUNING__
#include "bt_sink_srv_action.h"
#endif /* __BT_SINK_AUDIO_TUNING__ */

#ifdef __BT_SINK_SRV_AUDIO_TUNING__
#include "bt_sink_srv_utils.h"
#include "atci.h"
#include "hal_audio.h"
#include "bt_codec.h"
#include "audio_dsp_fd216_db_to_gain_value_mapping_table.h"
#include "bt_sink_srv_ami.h"
#include "audio_nvdm.h"
#include "audio_coefficient.h"

static bt_sink_srv_audio_tunning_context_t g_audio_tunning_ctx;

bt_sink_srv_tunning_func f_on = NULL;
bt_sink_srv_tunning_func f_off = NULL;


extern bt_media_handle_t *g_prHfp_media_handle;
extern bt_media_handle_t *g_prA2dp_sink_handle;
extern const uint32_t g_volume_out_config[DEVICE_OUT_MAX][AUD_VOL_TYPE][AUD_VOL_OUT_MAX][2];
extern const uint32_t g_volume_in_config[DEVICE_IN_MAX][AUD_VOL_IN_MAX][2];
extern bt_sink_srv_am_background_t *g_prCurrent_player;

static atci_cmd_hdlr_item_t bt_sink_srv_audio_tuning_cmd = {
    .command_head = "AT+AUDTUNNING",    /**< AT command string. */
    .command_hdlr = bt_sink_srv_audio_tuning_atci_handler,
    .hash_value1 = 0,
    .hash_value2 = 0,
};


/*******************************************************
AT+AUDTUNNING=VOL level_in level_out

AT+AUDTUNNING=DEV XXX stream_in stream_out

ref hal_audio.h
device stream out map
0        HAL_AUDIO_DEVICE_HEADSET = 0x0020
1        HAL_AUDIO_DEVICE_HANDS_FREE_STEREO  = 0x0010


device stream in map
0        HAL_AUDIO_DEVICE_MAIN_MIC           = 0x0001
1        HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC   = 0x0100
2        HAL_AUDIO_DEVICE_HEADSET_MIC        = 0x0002
*******************************************************/

#ifndef MTK_DEBUG_LEVEL_INFO
extern int32_t g_test_fix_warning;
#endif

bt_sink_srv_audio_tunning_context_t *bt_sink_srv_audio_tunning_get_context(void)
{
    return &g_audio_tunning_ctx;
}


void bt_sink_srv_audio_tunning_init(void)
{
    memset(&g_audio_tunning_ctx, 0x00, sizeof(bt_sink_srv_audio_tunning_context_t));

    g_audio_tunning_ctx.flag = TUNNING_FLAG_INIT;

    bt_sink_srv_audio_tunning_atci_init();

    //audio_nvdm_register_eaps_is_changed_callback((audio_nvdm_callback_id *)&g_audio_tunning_ctx, bt_sink_srv_audio_tunning_update_callback, NULL);

    /* set default vol & dev */
    /* a2dp default setting */
    g_audio_tunning_ctx.a2dp.mask |= TUNNING_MASK_DEV;
    g_audio_tunning_ctx.a2dp.dev_out = HAL_AUDIO_DEVICE_HEADSET;

    g_audio_tunning_ctx.a2dp.mask |= TUNNING_MASK_VOL;
    g_audio_tunning_ctx.a2dp.vol_out.level = 6;
    g_audio_tunning_ctx.a2dp.vol_out.digital = g_volume_out_config[1][0][6][0];
    g_audio_tunning_ctx.a2dp.vol_out.analog = g_volume_out_config[1][0][6][1];
    /* hf default setting */
    g_audio_tunning_ctx.hf.mask |= TUNNING_MASK_DEV;
#ifdef MTK_DEVELOPMENT_BOARD_HDK
    g_audio_tunning_ctx.hf.dev_in = HAL_AUDIO_DEVICE_MAIN_MIC;
#else
    g_audio_tunning_ctx.hf.dev_in = HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC;
#endif
    g_audio_tunning_ctx.hf.dev_out = HAL_AUDIO_DEVICE_HEADSET;

    g_audio_tunning_ctx.hf.mask |= TUNNING_MASK_VOL;
    g_audio_tunning_ctx.hf.vol_in.level = 0;
    g_audio_tunning_ctx.hf.vol_in.digital = g_volume_in_config[0][0][0];
    g_audio_tunning_ctx.hf.vol_in.analog = g_volume_in_config[0][0][1];
    g_audio_tunning_ctx.hf.vol_out.level = 6;
    g_audio_tunning_ctx.hf.vol_out.digital = g_volume_out_config[1][1][6][0];
    g_audio_tunning_ctx.hf.vol_out.analog = g_volume_out_config[1][1][6][1];
}


void bt_sink_srv_audio_tunning_atci_init(void)
{
    atci_status_t ret;

    ret = atci_register_handler(&bt_sink_srv_audio_tuning_cmd, sizeof(bt_sink_srv_audio_tuning_cmd) / sizeof(atci_cmd_hdlr_item_t));

    if (ret != ATCI_STATUS_OK) {
        bt_sink_srv_report("bt sink audio tunnning register fail");
    }
}


atci_status_t bt_sink_srv_audio_tuning_atci_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    int16_t result;

    response.response_flag = 0; /* Command Execute Finish. */

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+AUDTUNNING=<command> */
            result = bt_sink_srv_audio_tuning_command_entry(parse_cmd->string_ptr + 14);
            if (result == 0) {
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else {
                snprintf((char *)response.response_buf,
                         ATCI_UART_TX_FIFO_BUFFER_SIZE,
                         "command error:%d\r\n",
                         result);
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;

        default :
            /* others are invalid command format */
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
    return ATCI_STATUS_OK;
}


int16_t bt_sink_srv_audio_tuning_command_entry(char *string)
{
    int16_t result = -1;
    int32_t ret = 0;
    int32_t index_in = -1, index_out = -1;
    bt_sink_srv_audio_tunning_context_t *aud_tunning_p = NULL;
    audio_eaps_t *am_speech_eaps = NULL;
    uint8_t vol_lev = 0;
    uint16_t gain = 0;
    uint8_t mode = 0;


    bt_sink_srv_report("[Sink][Audio Tunning]command: %s", string);

    aud_tunning_p = bt_sink_srv_audio_tunning_get_context();

    am_speech_eaps = (audio_eaps_t *)pvPortMalloc(sizeof(audio_eaps_t));

    if (am_speech_eaps) {
        audio_nvdm_get_eaps_data_by_memcpy(am_speech_eaps);
    } else {
        bt_sink_srv_report("[Sink][Audio Tunning]command_entry no memory--ply: 0x%x, type: %d", g_prCurrent_player, g_prCurrent_player->type);
        ret = -200;
    }

    if (0 == strncmp(string, "VOL", 3)) {
        if (g_prCurrent_player && (g_prCurrent_player->type == A2DP ||
                                   g_prCurrent_player->type == HFP)) {
            if (g_prCurrent_player->type == A2DP) {
                /*************
                    VOL 0 6
                *************/
                index_out = string[6] - '0';
                aud_tunning_p->a2dp.mask |= TUNNING_MASK_VOL;
                aud_tunning_p->a2dp.vol_out.level = (uint8_t) index_out;
                if (am_speech_eaps) {
                    if (HAL_AUDIO_DEVICE_HEADSET == aud_tunning_p->a2dp.dev_out) {
                        vol_lev = aud_tunning_p->a2dp.vol_out.level;
                        #if PRODUCT_VERSION == 2533
                        gain = am_speech_eaps->audio_parameter.handsfree_headset_stream_out_gain.headset_gain[vol_lev];
                        #else
                        gain = am_speech_eaps->audio_parameter.audio_handsfree_headset_gain_parameter.headset_gain.stream_out_gain[vol_lev];
                        #endif
                        tunning_stream_out_db_transfer(gain,
                                                       &(aud_tunning_p->a2dp.vol_out.digital),
                                                       &(aud_tunning_p->a2dp.vol_out.analog));
                        bt_sink_srv_audio_tunning_update(TUNNING_SCENARIO_A2DP, TUNNING_TYPE_VOL);
                    } else if (HAL_AUDIO_DEVICE_HANDS_FREE_STEREO == aud_tunning_p->a2dp.dev_out) {
                        vol_lev = aud_tunning_p->a2dp.vol_out.level;
                        #if PRODUCT_VERSION == 2533
                        gain = am_speech_eaps->audio_parameter.handsfree_headset_stream_out_gain.handsfree_gain[vol_lev];
                        #else
                        gain = am_speech_eaps->audio_parameter.audio_handsfree_headset_gain_parameter.handsfree_gain.stream_out_gain[vol_lev];
                        #endif
						tunning_stream_out_db_transfer(gain,
                                                       &(aud_tunning_p->a2dp.vol_out.digital),
                                                       &(aud_tunning_p->a2dp.vol_out.analog));
                        bt_sink_srv_audio_tunning_update(TUNNING_SCENARIO_A2DP, TUNNING_TYPE_VOL);
                    }
                }
            } else if (g_prCurrent_player->type == HFP) {
                index_in = string[4] - '0';
                index_out = string[6] - '0';
                aud_tunning_p->hf.mask |= TUNNING_MASK_VOL;
                aud_tunning_p->hf.vol_in.level = (uint8_t) index_in;
                aud_tunning_p->hf.vol_out.level = (uint8_t) index_out;

                if (BT_HFP_CODEC_TYPE_CVSD == g_prCurrent_player->local_context.hfp_format.hfp_codec.type) {
                    if (HAL_AUDIO_DEVICE_HEADSET == aud_tunning_p->hf.dev_out) {
                        mode = SPEECH_MODE_HEADSET;
                    } else if (HAL_AUDIO_DEVICE_HANDS_FREE_STEREO == aud_tunning_p->hf.dev_out) {
                        mode = SPEECH_MODE_HANDSFREE;
                    }

                    #if PRODUCT_VERSION == 2533
                    mode = SPEECH_MODE_HEADSET_DUALMIC;
                    #endif
                    vol_lev = aud_tunning_p->hf.vol_in.level;
                    #if PRODUCT_VERSION == 2533
                    gain = am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[mode].voice_nb_gain.stream_in_gain[vol_lev];
                    #else
                    gain = am_speech_eaps->voice_parameter.voice_nb_parameter[mode].voice_nb_gain.stream_in_gain[vol_lev];
                    #endif
                    tunning_voice_uplink_db_transfer(gain,
                                                     &(aud_tunning_p->hf.vol_in.digital),
                                                     &(aud_tunning_p->hf.vol_in.analog));

                    vol_lev = aud_tunning_p->hf.vol_out.level;
                    #if PRODUCT_VERSION == 2533
                    gain = am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[mode].voice_nb_gain.stream_out_gain[vol_lev];
                    #else
                    gain = am_speech_eaps->voice_parameter.voice_nb_parameter[mode].voice_nb_gain.stream_out_gain[vol_lev];
                    #endif
                    tunning_stream_out_db_transfer(gain,
                                                   &(aud_tunning_p->hf.vol_out.digital),
                                                   &(aud_tunning_p->hf.vol_out.analog));
                    bt_sink_srv_audio_tunning_update(TUNNING_SCENARIO_HF, TUNNING_TYPE_VOL);
                } else if (BT_HFP_CODEC_TYPE_MSBC == g_prCurrent_player->local_context.hfp_format.hfp_codec.type) {
                    if (HAL_AUDIO_DEVICE_HEADSET == aud_tunning_p->hf.dev_out) {
                        mode = SPEECH_MODE_HEADSET;
                    } else if (HAL_AUDIO_DEVICE_HANDS_FREE_STEREO == aud_tunning_p->hf.dev_out) {
                        mode = SPEECH_MODE_HANDSFREE;
                    }

                    #if PRODUCT_VERSION == 2533
                    mode = SPEECH_MODE_HEADSET_DUALMIC;
                    #endif

                    vol_lev = aud_tunning_p->hf.vol_in.level;
                    #if PRODUCT_VERSION == 2533
                    gain = am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[mode].voice_wb_gain.stream_in_gain[vol_lev];
                    #else
                    gain = am_speech_eaps->voice_parameter.voice_wb_parameter[mode].voice_wb_gain.stream_in_gain[vol_lev];
                    #endif
                    tunning_voice_uplink_db_transfer(gain,
                                                     &(aud_tunning_p->hf.vol_in.digital),
                                                     &(aud_tunning_p->hf.vol_in.analog));

                    vol_lev = aud_tunning_p->hf.vol_out.level;
                    #if PRODUCT_VERSION == 2533
                    gain = am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[mode].voice_wb_gain.stream_out_gain[vol_lev];
                    #else
                    gain = am_speech_eaps->voice_parameter.voice_wb_parameter[mode].voice_wb_gain.stream_out_gain[vol_lev];
                    #endif
                    tunning_stream_out_db_transfer(gain,
                                                   &(aud_tunning_p->hf.vol_out.digital),
                                                   &(aud_tunning_p->hf.vol_out.analog));
                    bt_sink_srv_audio_tunning_update(TUNNING_SCENARIO_HF, TUNNING_TYPE_VOL);
                }
            }
        } else {
            ret = -100;
            bt_sink_srv_report("[Sink][Audio Tunning]error--ply: 0x%x, type: %d", g_prCurrent_player, g_prCurrent_player->type);
        }
#if 0
        /* tunning volume */
        if (0 == strncmp(string + 4, "HF", 2)) {
            if (10 == strlen(string)) {
                /*************
                    VOL HF 0 6
                *************/
                index_in = string[7] - '0';
                index_out = string[9] - '0';

                /* update */
                aud_tunning_p->hf.mask |= TUNNING_MASK_VOL;
                aud_tunning_p->hf.vol_in.level = (uint8_t) index_in;
                aud_tunning_p->hf.vol_out.level = (uint8_t) index_out;
                if (aud_tunning_p->hf.mask & TUNNING_MASK_DEV) {
                }
            } else {
                ret = -1;
            }
        } else if (0 == strncmp(string + 4, "A2DP", 4)) {
            if (10 == strlen(string)) {
                /*************
                    VOL A2DP 2
                *************/
                index_out = string[9] - '0';

                /* update */
                aud_tunning_p->a2dp.mask |= TUNNING_MASK_VOL;
                aud_tunning_p->a2dp.vol_out.level = (uint8_t) index_out;
            } else {
                ret = -2;
            }
        }
#endif
        result = 0;
    } else if (0 == strncmp(string, "DEV", 3)) {
        /* tunning stream in/out device */
        if (0 == strncmp(string + 4, "HF", 2)) {
            if (12 == strlen(string)) {
                //if (true) {
                /*************
                    DEV HF 0 6
                *************/
                index_in = string[7] - '0';
                index_out = string[9] - '0';

                /* update */
                aud_tunning_p->hf.mask |= TUNNING_MASK_DEV;
                aud_tunning_p->hf.dev_in = bt_sink_srv_get_tunning_dev(index_in, TUNNING_DEV_IN);
                aud_tunning_p->hf.dev_out = bt_sink_srv_get_tunning_dev(index_out, TUNNING_DEV_OUT);

                bt_sink_srv_audio_tunning_update(TUNNING_SCENARIO_HF, TUNNING_TYPE_DEV);
            } else {
                ret = -3;
            }
        } else if (0 == strncmp(string + 4, "A2DP", 4)) {
            if (12 == strlen(string)) {
                //if (true) {
                /*************
                    DEV A2DP 2
                *************/
                index_out = string[9] - '0';

                /* update */
                aud_tunning_p->a2dp.mask |= TUNNING_MASK_DEV;
                aud_tunning_p->a2dp.dev_out = bt_sink_srv_get_tunning_dev(index_out, TUNNING_DEV_OUT);
                bt_sink_srv_audio_tunning_update(TUNNING_SCENARIO_A2DP, TUNNING_TYPE_DEV);
            } else {
                ret = -4;
            }
        }
        result = 0;
    }

    bt_sink_srv_report("[Sink][Audio Tunning]ret: %d, in: %d, out: %d",
                       ret, index_in, index_out);

    if (am_speech_eaps) {
        vPortFree(am_speech_eaps);
    }

    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = ret;
    #endif
    return result;
}


hal_audio_device_t bt_sink_srv_get_tunning_dev(int32_t index, uint8_t type)
{
    hal_audio_device_t dev = HAL_AUDIO_DEVICE_NONE;

    if (TUNNING_DEV_IN == type) {
        switch (index) {
            case 0: {
                dev = HAL_AUDIO_DEVICE_MAIN_MIC;
                break;
            }

            case 1: {
                dev = HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC;
                break;
            }

            case 2: {
                dev = HAL_AUDIO_DEVICE_HEADSET_MIC;
                break;
            }

            default:
                break;
        }
    } else if (TUNNING_DEV_OUT == type) {
        switch (index) {
            case 0: {
                dev = HAL_AUDIO_DEVICE_HEADSET;
                break;
            }

            case 1: {
                dev = HAL_AUDIO_DEVICE_HANDS_FREE_STEREO;
                break;
            }

            default:
                break;
        }
    }

    bt_sink_srv_report("[Sink][Audio Tunning]get_dev--dev: 0x%x, index: %d, type: %d", dev, index, type);

    return dev;
}


void bt_sink_srv_audio_tunning_update_callback(void *data)
{
    audio_eaps_t *am_speech_eaps = NULL;
    uint8_t vol_lev = 0;
    uint16_t gain = 0;
    uint8_t mode = 0;
    int32_t ret = 0;
    bt_sink_srv_audio_tunning_context_t *aud_tunning_p = NULL;


    aud_tunning_p = bt_sink_srv_audio_tunning_get_context();

    am_speech_eaps = (audio_eaps_t *)pvPortMalloc(sizeof(audio_eaps_t));

    if (am_speech_eaps) {
        audio_nvdm_get_eaps_data_by_memcpy(am_speech_eaps);
    } else {
        bt_sink_srv_report("[Sink][Audio Tunning]update_callback no memory--ply: 0x%x, type: %d", g_prCurrent_player, g_prCurrent_player->type);
        ret = -200;
    }

    if (am_speech_eaps) {
        if (HAL_AUDIO_DEVICE_HEADSET == aud_tunning_p->a2dp.dev_out) {
            vol_lev = aud_tunning_p->a2dp.vol_out.level;
            #if PRODUCT_VERSION == 2533
            gain = am_speech_eaps->audio_parameter.handsfree_headset_stream_out_gain.headset_gain[vol_lev];
            #else
            gain = am_speech_eaps->audio_parameter.audio_handsfree_headset_gain_parameter.headset_gain.stream_out_gain[vol_lev];
            #endif
            tunning_stream_out_db_transfer(gain,
                                           &(aud_tunning_p->a2dp.vol_out.digital),
                                           &(aud_tunning_p->a2dp.vol_out.analog));
        } else if (HAL_AUDIO_DEVICE_HANDS_FREE_STEREO == aud_tunning_p->a2dp.dev_out) {
            vol_lev = aud_tunning_p->a2dp.vol_out.level;
            #if PRODUCT_VERSION == 2533
            gain = am_speech_eaps->audio_parameter.handsfree_headset_stream_out_gain.handsfree_gain[vol_lev];
            #else
            gain = am_speech_eaps->audio_parameter.audio_handsfree_headset_gain_parameter.handsfree_gain.stream_out_gain[vol_lev];
            #endif
            tunning_stream_out_db_transfer(gain,
                                           &(aud_tunning_p->a2dp.vol_out.digital),
                                           &(aud_tunning_p->a2dp.vol_out.analog));
        }
    }

    if (am_speech_eaps && g_prCurrent_player && g_prCurrent_player->type == HFP) {
        if (BT_HFP_CODEC_TYPE_CVSD == g_prCurrent_player->local_context.hfp_format.hfp_codec.type) {
            if (HAL_AUDIO_DEVICE_HEADSET == aud_tunning_p->hf.dev_out) {
                mode = SPEECH_MODE_HEADSET;
            } else if (HAL_AUDIO_DEVICE_HANDS_FREE_STEREO == aud_tunning_p->hf.dev_out) {
                mode = SPEECH_MODE_HANDSFREE;
            }

            #if PRODUCT_VERSION == 2533
            mode = SPEECH_MODE_HEADSET_DUALMIC;
            #endif

            vol_lev = aud_tunning_p->hf.vol_in.level;
            #if PRODUCT_VERSION == 2533
            gain = am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[mode].voice_nb_gain.stream_in_gain[vol_lev];
            #else
            gain = am_speech_eaps->voice_parameter.voice_nb_parameter[mode].voice_nb_gain.stream_in_gain[vol_lev];
            #endif
            tunning_voice_uplink_db_transfer(gain,
                                             &(aud_tunning_p->hf.vol_in.digital),
                                             &(aud_tunning_p->hf.vol_in.analog));

            vol_lev = aud_tunning_p->hf.vol_out.level;
            #if PRODUCT_VERSION == 2533
            gain = am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[mode].voice_nb_gain.stream_out_gain[vol_lev];
            #else
            gain = am_speech_eaps->voice_parameter.voice_nb_parameter[mode].voice_nb_gain.stream_out_gain[vol_lev];
            #endif
            tunning_stream_out_db_transfer(gain,
                                           &(aud_tunning_p->hf.vol_out.digital),
                                           &(aud_tunning_p->hf.vol_out.analog));
        } else if (BT_HFP_CODEC_TYPE_MSBC == g_prCurrent_player->local_context.hfp_format.hfp_codec.type) {
            if (HAL_AUDIO_DEVICE_HEADSET == aud_tunning_p->hf.dev_out) {
                mode = SPEECH_MODE_HEADSET;
            } else if (HAL_AUDIO_DEVICE_HANDS_FREE_STEREO == aud_tunning_p->hf.dev_out) {
                mode = SPEECH_MODE_HANDSFREE;
            }

            #if PRODUCT_VERSION == 2533
            mode = SPEECH_MODE_HEADSET_DUALMIC;
            #endif

            vol_lev = aud_tunning_p->hf.vol_in.level;
            vol_lev = aud_tunning_p->hf.vol_out.level;

            #if PRODUCT_VERSION == 2533
            gain = am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[mode].voice_wb_gain.stream_in_gain[vol_lev];
            #else
            gain = am_speech_eaps->voice_parameter.voice_wb_parameter[mode].voice_wb_gain.stream_in_gain[vol_lev];
            #endif
            tunning_voice_uplink_db_transfer(gain,
                                             &(aud_tunning_p->hf.vol_in.digital),
                                             &(aud_tunning_p->hf.vol_in.analog));

            vol_lev = aud_tunning_p->hf.vol_out.level;
            #if PRODUCT_VERSION == 2533
            gain = am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[mode].voice_wb_gain.stream_out_gain[vol_lev];
            #else
            gain = am_speech_eaps->voice_parameter.voice_wb_parameter[mode].voice_wb_gain.stream_out_gain[vol_lev];
            #endif
            tunning_stream_out_db_transfer(gain,
                                           &(aud_tunning_p->hf.vol_out.digital),
                                           &(aud_tunning_p->hf.vol_out.analog));
        }
    }

    if (g_prCurrent_player) {
        if (g_prCurrent_player->type == A2DP) {
            bt_sink_srv_audio_tunning_update(TUNNING_SCENARIO_A2DP, TUNNING_TYPE_VOL);
        } else if (g_prCurrent_player->type == HFP) {
            bt_sink_srv_audio_tunning_update(TUNNING_SCENARIO_HF, TUNNING_TYPE_VOL);
        }
    }

    if (am_speech_eaps != NULL) {
        vPortFree(am_speech_eaps);
    }

    bt_sink_srv_report("[Sink][Audio Tunning]update_callback--ret: %d", ret);    
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = ret;
    #endif
}


void bt_sink_srv_audio_tunning_update(uint8_t scen, uint8_t type)
{
    int32_t ret1 = 0, ret2 = 0;
    bt_sink_srv_audio_tunning_context_t *aud_tunning_p = NULL;

    aud_tunning_p = bt_sink_srv_audio_tunning_get_context();

    switch (scen) {
        case TUNNING_SCENARIO_A2DP: {
            if (TUNNING_TYPE_VOL == type) {
                ret1 = hal_audio_set_stream_out_volume(aud_tunning_p->a2dp.vol_out.digital, aud_tunning_p->a2dp.vol_out.analog);
            } else if (TUNNING_TYPE_DEV == type) {
                ret1 = hal_audio_set_stream_out_device((hal_audio_device_t)aud_tunning_p->a2dp.dev_out);
            }
            break;
        }

        case TUNNING_SCENARIO_HF: {
            if (TUNNING_TYPE_VOL == type) {
                ret1 = hal_audio_set_stream_in_volume(aud_tunning_p->hf.vol_in.digital, aud_tunning_p->hf.vol_in.analog);
                ret2 = hal_audio_set_stream_out_volume(aud_tunning_p->hf.vol_out.digital, aud_tunning_p->hf.vol_out.analog);
            } else if (TUNNING_TYPE_DEV == type) {
                ret1 = hal_audio_set_stream_in_device((hal_audio_device_t)aud_tunning_p->hf.dev_in);
                ret2 = hal_audio_set_stream_out_device((hal_audio_device_t)aud_tunning_p->hf.dev_out);
            }
            break;
        }

        default:
            break;
    }

    bt_sink_srv_report("[Sink][Audio Tunning]update--ret1: %d, ret2: %d, scen: %d, type: %d", ret1, ret2, scen, type);
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = ret1;
    g_test_fix_warning = ret2;
    #endif
}


void tunning_stream_out_db_transfer(uint16_t gain, uint32_t *digital, uint32_t *analog)
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

    bt_sink_srv_report("[Sink][Audio Tunning]stream_db--gain: %d, digital: %d, analog: %d", gain, digital, analog);
}


void tunning_voice_uplink_db_transfer(uint16_t gain, uint32_t *digital, uint32_t *analog)
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

    bt_sink_srv_report("[Sink][Audio Tunning]voice_db--gain: %d, digital: %d, analog: %d", gain, digital, analog);
}

#endif /* __BT_SINK_SRV_AUDIO_TUNING__ */

#ifdef __BT_SINK_AUDIO_TUNING__

//extern void bt_common_callback_set_demo(bt_demo_t demo);

static atci_status_t bt_sink_hf_audio_tuning_atci_handler(atci_parse_cmd_param_t *parse_cmd);

static atci_cmd_hdlr_item_t bt_sink_audio_tuning_cmd = {
    .command_head = "AT+HFTUNNING",    /**< AT command string. */
    .command_hdlr = bt_sink_hf_audio_tuning_atci_handler,
    .hash_value1 = 0,
    .hash_value2 = 0,
};

static int16_t bt_sink_hf_audio_tuning_command_entry(char *string)
{
#ifdef __BT_SINK_SRV_AUDIO_TUNING__
    bt_sink_srv_audio_tunning_context_t *aud_tunning_p = NULL;
#endif
    int16_t result = -1;

    bt_sink_srv_report("[Sink][HF][TUNING] command:%s", string);

#ifdef __BT_SINK_SRV_AUDIO_TUNING__
    aud_tunning_p = bt_sink_srv_audio_tunning_get_context();
#endif

    if (0 == bt_sink_srv_memcmp(string, "TUNING_ON", strlen("TUNING_ON"))) {
        //bt_common_callback_set_demo(BT_AUDIO);
        //bt_sink_event_post(BT_SINK_EVENT_CM_POWER_SWITCH, NULL, NULL);
        //bt_demo_power_on();
        if (f_on) {
            f_on();
#ifdef __BT_SINK_SRV_AUDIO_TUNING__
            aud_tunning_p->flag |= TUNNING_FLAG_ON;
#endif
        }
        result = 0;
    } else if (0 == bt_sink_srv_memcmp(string, "TUNING_OFF", strlen("TUNING_OFF"))) {
        //bt_sink_event_send(BT_SINK_EVENT_CM_POWER_SWITCH, NULL);
        //bt_demo_power_off();
        //bt_common_callback_set_demo(BT_IDLE);
        if (f_off) {
            f_off();
#ifdef __BT_SINK_SRV_AUDIO_TUNING__
            aud_tunning_p->flag &= (~TUNNING_FLAG_ON);
#endif
        }
        result = 0;
    } else if (0 == bt_sink_srv_memcmp(string, "ANSWER", strlen("ANSWER"))) {
        //bt_sink_event_post(BT_SINK_EVENT_HF_ANSWER, NULL, NULL);
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_ANSWER, NULL);
        result = 0;
    } else if (0 == bt_sink_srv_memcmp(string, "HANG_UP", strlen("HANG_UP"))) {
        //bt_sink_event_post(BT_SINK_EVENT_HF_HANG_UP, NULL, NULL);
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_HANG_UP, NULL);
        result = 0;
    }
    return result;
}

atci_status_t bt_sink_hf_audio_tuning_atci_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    int16_t result;

    response.response_flag = 0; /* Command Execute Finish. */

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+HFTUNNING=<command> */
            result = bt_sink_hf_audio_tuning_command_entry(parse_cmd->string_ptr + 13);
            if (result == 0) {
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else {
                snprintf((char *)response.response_buf,
                         ATCI_UART_TX_FIFO_BUFFER_SIZE,
                         "command error:%d\r\n",
                         result);
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;

        default :
            /* others are invalid command format */
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
    return ATCI_STATUS_OK;
}

void bt_sink_hf_audio_tuning_atci_init(void)
{
    atci_status_t ret;

    ret = atci_register_handler(&bt_sink_audio_tuning_cmd, sizeof(bt_sink_audio_tuning_cmd) / sizeof(atci_cmd_hdlr_item_t));

    if (ret != ATCI_STATUS_OK) {
        bt_sink_srv_report("at_cmd_init register fail");
    }
}


void bt_sink_srv_set_tunning_func(bt_sink_srv_tunning_func tunning_on,
                                  bt_sink_srv_tunning_func tunning_off)
{
    f_on = tunning_on;
    f_off = tunning_off;
}

#endif /* __BT_SINK_AUDIO_TUNING__ */

