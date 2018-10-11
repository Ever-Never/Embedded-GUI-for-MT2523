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

#include <stdio.h>
#include "syslog.h"
#include "audio_nvdm.h"
#include "FreeRTOS.h"
#include "hal_audio_enhancement.h"
#include "audio_middleware_api.h"



log_create_module(audio_middleware_api, PRINT_LEVEL_INFO);

#define LOGE(fmt,arg...)   LOG_E(audio_middleware_api, "[audio_middleware_api]"fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(audio_middleware_api, "[audio_middleware_api]"fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(audio_middleware_api, "[audio_middleware_api]"fmt,##arg)


#define SPEECH_MODE_HEADSET 0


void audio_middleware_set_default_enhancement_parameters()
{
    // set speech headset mode as default enhancement parameters
    LOGI("audio middleware set default enhancement parameters ++\n");

    const audio_eaps_t *eaps;
    eaps = audio_nvdm_get_global_eaps_address();

#if PRODUCT_VERSION != 2533 // TODO: not using this method, we may set default enhancement in spicific file
    speech_update_common(eaps->speech_common_parameter.speech_common_parameter);
    speech_update_nb_fir((int16_t *)eaps->voice_parameter.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient,
                         (int16_t *)eaps->voice_parameter.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient);
    speech_update_nb_param(eaps->voice_parameter.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_enhancement_parameter.nb_mode_parameter);
    speech_update_wb_fir((int16_t *)eaps->voice_parameter.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_enhancement_parameter.wb_stream_in_fir_coefficient,
                         (int16_t *)eaps->voice_parameter.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient);
    speech_update_wb_param(eaps->voice_parameter.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_enhancement_parameter.wb_mode_parameter);
#else
    speech_update_common(eaps->speech_common_parameter.speech_common_parameter);
    speech_update_nb_fir((int16_t *)eaps->voice_parameter.voice_nb_band.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient,
                         (int16_t *)eaps->voice_parameter.voice_nb_band.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient);
    speech_update_nb_param(eaps->voice_parameter.voice_nb_band.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_enhancement_parameter.nb_mode_parameter);
    speech_update_wb_fir((int16_t *)eaps->voice_parameter.voice_wb_band.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_enhancement_parameter.wb_stream_in_fir_coefficient,
                         (int16_t *)eaps->voice_parameter.voice_wb_band.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient);
    speech_update_wb_param(eaps->voice_parameter.voice_wb_band.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_enhancement_parameter.wb_mode_parameter);    
#endif


    LOGI("audio middleware set default enhancement parameters --\n");

}

int audio_middleware_init()
{
    int ret = 0;

    LOGI("audio middleware init ++\n");
    
    audio_nvdm_init();

    audio_middleware_set_default_enhancement_parameters();

    LOGI("audio middleware init --\n");

    return ret;
}

