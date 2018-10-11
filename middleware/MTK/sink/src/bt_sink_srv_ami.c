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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "bt_sink_srv_am_task.h"
#include "bt_sink_srv_ami.h"

#ifdef __AM_DEBUG_INFO__
#include "bt_sink_srv_utils.h"
#endif

#ifdef MTK_EXTERNAL_DSP_ENABLE
#include "external_dsp_application.h"
#endif

//#define ANALOG_VOL_MAX     10 /*depend on HAL*/
//#define DIGITAL_VOL_MAX    10 /*depend on HAL*/

bt_sink_srv_am_background_t *g_prCurrent_player = NULL;
bt_sink_srv_am_id_t g_aud_id_num = 0;
bt_bd_addr_t g_int_dev_addr = {0};

#ifndef WIN32_UT
xSemaphoreHandle g_xSemaphore_ami = NULL;
void ami_mutex_lock(xSemaphoreHandle handle)
{
    if (handle != NULL) {
        xSemaphoreTake(handle, portMAX_DELAY);
    }
}
void ami_mutex_unlock(xSemaphoreHandle handle)
{
    if (handle != NULL) {
        xSemaphoreGive(handle);
    }
}
#else
bt_sink_srv_am_amm_struct *g_prAmm_current = NULL;
#define ami_mutex_lock()   { }
#define ami_mutex_unlock() { }
#endif

/*****************************************************************************
 * FUNCTION
 *  ami_register_get_id
 * DESCRIPTION
 *  Get the redistered ID
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_sink_srv_am_id_t
 *****************************************************************************/
static bt_sink_srv_am_id_t ami_register_get_id(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_id_t bAud_id = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    while (g_rAm_aud_id[bAud_id].use != ID_CLOSE_STATE) {
        bAud_id++;
    }
    g_rAm_aud_id[bAud_id].use = ID_IDLE_STATE;
    //g_rAm_aud_id[bAud_id].contain_ptr = NULL;
    g_aud_id_num++;
    return bAud_id;
}

/*****************************************************************************
 * FUNCTION
 *  ami_register_check_id_exist
 * DESCRIPTION
 *  Check if the specified ID is valid
 * PARAMETERS
 *  aud_id			[IN]
 * RETURNS
 *  bt_sink_srv_am_id_t
 *****************************************************************************/
 #if 0
static bt_sink_srv_am_id_t ami_register_check_id_exist(bt_sink_srv_am_id_t aud_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((aud_id < AM_REGISTER_ID_TOTAL) && (g_rAm_aud_id[aud_id].use != ID_CLOSE_STATE)) {
        return TRUE;
    }
    return FALSE;
}
#endif


/*****************************************************************************
 * FUNCTION
 *  ami_register_delete_id
 * DESCRIPTION
 *  Delete redistered ID
 * PARAMETERS
 *  aud_id           [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void ami_register_delete_id(bt_sink_srv_am_id_t aud_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_rAm_aud_id[aud_id].use != ID_CLOSE_STATE) {
        g_rAm_aud_id[aud_id].use = ID_CLOSE_STATE;
        g_aud_id_num--;
    }
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_audio_open
 * DESCRIPTION
 *  Use this function to open a new audio handler and get the audio ID.
 * PARAMETERS
 *  priority         [IN]
 *  handler          [IN]
 * RETURNS
 *  bt_sink_srv_am_id_t
 *****************************************************************************/
bt_sink_srv_am_id_t bt_sink_srv_ami_audio_open(bt_sink_srv_am_priority_t priority,
        bt_sink_srv_am_notify_callback handler)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};
    bt_sink_srv_am_id_t bAud_id;
    int32_t pri = 0;
    //bt_sink_srv_am_hal_result_t result = HAL_AUDIO_STATUS_ERROR;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_sink_srv_report("[AudM]open-pri: %d, num: %d, cb: 0x%x", priority, g_aud_id_num, (unsigned int)handler);
    //printf("[AudM]open-pri: %d, num: %d, cb: 0x%x\n", priority, g_aud_id_num, (unsigned int)handler);

    pri = (int32_t)priority;
    if ((g_aud_id_num < AM_REGISTER_ID_TOTAL) && (pri >= AUD_LOW) && (pri <= AUD_HIGH)) {
        ami_mutex_lock(g_xSemaphore_ami);
        bAud_id = ami_register_get_id();
        ami_mutex_unlock(g_xSemaphore_ami);
#ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AMI] Open func, ID: %d, pri: %d, num: %d",
            bAud_id, priority, g_aud_id_num);
#endif
        temp_background_t.aud_id = bAud_id;
        temp_background_t.notify_cb = handler;
        temp_background_t.priority = priority;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_STREAM_OPEN_REQ, &temp_background_t,
                                 FALSE, NULL);
        return (bt_sink_srv_am_id_t)bAud_id;
    }
    return AUD_ID_INVALID;
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_audio_play
 * DESCRIPTION
 *  Start to play the specified audio handler.
 * PARAMETERS
 *  aud_id           [IN]
 *  capability_t     [IN]
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t bt_sink_srv_ami_audio_play(bt_sink_srv_am_id_t aud_id,
        bt_sink_srv_am_audio_capability_t *capability_t)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //printf("[AudM]play-id: %d, num: %d, use: %d, ptr: 0x%x, type: %d\n",
        //aud_id, g_aud_id_num, g_rAm_aud_id[aud_id].use,
        //(unsigned int)g_rAm_aud_id[aud_id].contain_ptr, capability_t->type);
    bt_sink_srv_report("[AudM]play-id: %d, num: %d, use: %d, ptr: 0x%x, type: %d",
        aud_id, g_aud_id_num, g_rAm_aud_id[aud_id].use,
        (unsigned int)g_rAm_aud_id[aud_id].contain_ptr, capability_t->type);
    if ((aud_id < AM_REGISTER_ID_TOTAL) &&
            ((g_rAm_aud_id[aud_id].use == ID_IDLE_STATE) || (g_rAm_aud_id[aud_id].use == ID_RESUME_STATE) || (g_rAm_aud_id[aud_id].use == ID_STOPING_STATE))) {
#ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AMI] Play func, ID: %d", aud_id);
#endif

#ifdef MTK_AUTOMOTIVE_SUPPORT
        external_dsp_echo_ref_sw_config(true);
#endif


        temp_background_t.aud_id = aud_id;
        temp_background_t.type = capability_t->type;
        temp_background_t.audio_path_type = capability_t->audio_path_type;
        memcpy(&(temp_background_t.local_context), &(capability_t->codec), sizeof(bt_sink_srv_am_codec_t));
        memcpy(&(temp_background_t.audio_stream_in), &(capability_t->audio_stream_in), sizeof(bt_sink_srv_am_audio_stream_in_t));
        memcpy(&(temp_background_t.audio_stream_out), &(capability_t->audio_stream_out), sizeof(bt_sink_srv_am_audio_stream_out_t));
        memcpy(&g_int_dev_addr, &(capability_t->dev_addr), sizeof(bt_bd_addr_t));
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_STREAM_PLAY_REQ, &temp_background_t,
                                 FALSE, NULL);
        return AUD_EXECUTION_SUCCESS;
    }
    return AUD_EXECUTION_FAIL;
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_audio_stop
 * DESCRIPTION
 *  Stop playing the specified audio handler.
 * PARAMETERS
 *  aud_id           [IN]
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t bt_sink_srv_ami_audio_stop(bt_sink_srv_am_id_t aud_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //printf("[AudM]stop-id: %d, num: %d, use: %d, ptr: 0x%x\n",
        //aud_id, g_aud_id_num, g_rAm_aud_id[aud_id].use, (unsigned int)g_rAm_aud_id[aud_id].contain_ptr);
    bt_sink_srv_report("[AudM]stop-id: %d, num: %d, use: %d, ptr: 0x%x",
        aud_id, g_aud_id_num, g_rAm_aud_id[aud_id].use, (unsigned int)g_rAm_aud_id[aud_id].contain_ptr);
    if ((aud_id < AM_REGISTER_ID_TOTAL) && (g_rAm_aud_id[aud_id].use == ID_PLAY_STATE)) {
#ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AMI] Stop func, ID: %d", aud_id);
#endif

#ifdef MTK_AUTOMOTIVE_SUPPORT
        external_dsp_echo_ref_sw_config(false);
#endif

        temp_background_t.aud_id = aud_id;
        g_rAm_aud_id[aud_id].use = ID_STOPING_STATE;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_STREAM_STOP_REQ, &temp_background_t,
                                 FALSE, NULL);
        return AUD_EXECUTION_SUCCESS;
    }
    return AUD_EXECUTION_FAIL;
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_audio_close
 * DESCRIPTION
 *  Close the audio handler opened.
 * PARAMETERS
 *  aud_id           [IN]
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t bt_sink_srv_ami_audio_close(bt_sink_srv_am_id_t aud_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //bt_sink_srv_report("[Sink][AMI] Pre-close func, ID: %d, use:%d", aud_id, g_rAm_aud_id[aud_id].use);
    //printf("[AudM]close-id: %d, num: %d, use: %d, ptr: 0x%x\n",
        //aud_id, g_aud_id_num, g_rAm_aud_id[aud_id].use, (unsigned int)g_rAm_aud_id[aud_id].contain_ptr);
    bt_sink_srv_report("[AudM]close-id: %d, num: %d, use: %d, ptr: 0x%x",
        aud_id, g_aud_id_num, g_rAm_aud_id[aud_id].use, (unsigned int)g_rAm_aud_id[aud_id].contain_ptr);
    if ((aud_id < AM_REGISTER_ID_TOTAL) &&
            ((g_rAm_aud_id[aud_id].use == ID_IDLE_STATE) ||
             (g_rAm_aud_id[aud_id].use == ID_SUSPEND_STATE) ||
             (g_rAm_aud_id[aud_id].use == ID_RESUME_STATE))) {
#ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AMI] Close func, ID: %d", aud_id);
#endif
        temp_background_t.aud_id = aud_id;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_STREAM_CLOSE_REQ, &temp_background_t,
                                 FALSE, NULL);
        ami_mutex_lock(g_xSemaphore_ami);
        ami_register_delete_id(aud_id);
        ami_mutex_unlock(g_xSemaphore_ami);
        return AUD_EXECUTION_SUCCESS;
    }
    return AUD_EXECUTION_FAIL;
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_audio_set_volume
 * DESCRIPTION
 *  Set audio input/output volume.
 * PARAMETERS
 * 	aud_id           [IN]
 *  volume_level     [IN]
 *  in_out           [IN]
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t bt_sink_srv_ami_audio_set_volume(bt_sink_srv_am_id_t aud_id,
        bt_sink_srv_am_volume_level_t volume_level,
        bt_sink_srv_am_stream_type_t in_out)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((aud_id < AM_REGISTER_ID_TOTAL) &&
            (g_rAm_aud_id[aud_id].use == ID_PLAY_STATE)) {

        temp_background_t.aud_id = aud_id;
        temp_background_t.in_out = in_out;
        if (in_out == STREAM_OUT) {
            if (volume_level < AUD_VOL_OUT_MAX) {
                temp_background_t.audio_stream_out.audio_volume = (bt_sink_srv_am_volume_level_out_t)volume_level;
            } else {
#ifdef __AM_DEBUG_INFO__
                bt_sink_srv_report("[Sink][AMI] Vol-level error");
#endif
                return AUD_EXECUTION_FAIL;
            }
        } else if (in_out == STREAM_IN) {
            if (volume_level < AUD_VOL_IN_MAX) {
                temp_background_t.audio_stream_in.audio_volume = (bt_sink_srv_am_volume_level_in_t)volume_level;
            } else {
#ifdef __AM_DEBUG_INFO__
                bt_sink_srv_report("[Sink][AMI] Vol-level error");
#endif
                return AUD_EXECUTION_FAIL;
            }
        }
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_STREAM_SET_VOLUME_REQ, &temp_background_t,
                                 FALSE, NULL);
        return AUD_EXECUTION_SUCCESS;
    }
    return AUD_EXECUTION_FAIL;
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_audio_set_mute
 * DESCRIPTION
 *  Mute audio input/output device.
 * PARAMETERS
 *  aud_id           [IN]
 *  mute             [IN]
 * in_out            [IN]
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t bt_sink_srv_ami_audio_set_mute(bt_sink_srv_am_id_t aud_id,
        bool mute,
        bt_sink_srv_am_stream_type_t in_out)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((aud_id < AM_REGISTER_ID_TOTAL) &&
            (g_rAm_aud_id[aud_id].use == ID_PLAY_STATE)) {
        temp_background_t.aud_id = aud_id;
        temp_background_t.in_out = in_out;
        if (in_out == STREAM_OUT) {
            temp_background_t.audio_stream_out.audio_mute = mute;
        } else if (in_out == STREAM_IN) {
            temp_background_t.audio_stream_in.audio_mute = mute;
        }
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_STREAM_MUTE_DEVICE_REQ, &temp_background_t,
                                 FALSE, NULL);
        return AUD_EXECUTION_SUCCESS;
    }
    return AUD_EXECUTION_FAIL;
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_audio_set_device
 * DESCRIPTION
 *  Set audio input/output device.
 * PARAMETERS
 *  aud_id           [IN]
 *  device           [IN]
 *  in_out           [IN]
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t bt_sink_srv_ami_audio_set_device(bt_sink_srv_am_id_t aud_id,
        bt_sink_srv_am_device_set_t device)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};
    bt_sink_srv_am_id_t bCount = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((aud_id < AM_REGISTER_ID_TOTAL) &&
            (g_rAm_aud_id[aud_id].use == ID_PLAY_STATE)) {
        while (device > 0) {
            if ((device & 1) == 1) {
                bCount++;
            }
            device >>= 1;
        }
        if (bCount > 1) {
            return AUD_EXECUTION_FAIL;
        }
        temp_background_t.aud_id = aud_id;
        if (device & DEVICE_OUT_LIST) {
            temp_background_t.in_out = STREAM_OUT;
            temp_background_t.audio_stream_out.audio_device = device;
        } else if (device & DEVICE_IN_LIST) {
            temp_background_t.in_out = STREAM_IN;
            temp_background_t.audio_stream_in.audio_device = device;
        }
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_STREAM_CONFIG_DEVICE_REQ, &temp_background_t,
                                 FALSE, NULL);
        return AUD_EXECUTION_SUCCESS;
    }
    return AUD_EXECUTION_FAIL;
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_audio_continue_stream
 * DESCRIPTION
 *  Continuously write data to audio output for palyback / read data from audio input for record.
 * PARAMETERS
 *  aud_id        	 [IN]
 *  buffer           [IN/OUT]
 *  data_count    	 [IN]
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t bt_sink_srv_ami_audio_continue_stream(bt_sink_srv_am_id_t aud_id,
        void *buffer,
        uint32_t data_count)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((aud_id < AM_REGISTER_ID_TOTAL) &&
            (g_rAm_aud_id[aud_id].contain_ptr->type == PCM) &&
            ((g_rAm_aud_id[aud_id].use == ID_PLAY_STATE) ||
             (g_rAm_aud_id[aud_id].use == ID_RESUME_STATE))) {
        temp_background_t.aud_id = aud_id;
        temp_background_t.local_context.pcm_format.stream.size = data_count;
        temp_background_t.local_context.pcm_format.stream.buffer = buffer;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_STREAM_READ_WRITE_DATA_REQ, &temp_background_t,
                                 FALSE, NULL);
        return AUD_EXECUTION_SUCCESS;
    }
    return AUD_EXECUTION_FAIL;
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_audio_get_stream_length
 * DESCRIPTION
 *  Query available input/output data length.
 * PARAMETERS
 *  aud_id		     [IN]
 *  data_length      [OUT]
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t bt_sink_srv_ami_audio_get_stream_length(bt_sink_srv_am_id_t aud_id,
        uint32_t *data_length,
        bt_sink_srv_am_stream_type_t in_out)
{
    #if 0
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (ami_register_check_id_exist(aud_id)) {
        temp_background_t.aud_id = aud_id;
        temp_background_t.in_out = in_out;
        temp_background_t.data_length_ptr = data_length;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_STREAM_GET_LENGTH_REQ, &temp_background_t,
                                 FALSE, NULL);
        return AUD_EXECUTION_SUCCESS;
    }
    return AUD_EXECUTION_FAIL;
    #endif
    bt_sink_srv_assert(data_length);
    if (STREAM_OUT == in_out) {
        hal_audio_get_stream_out_sample_count(data_length);
    } else if (STREAM_IN == in_out) {
        hal_audio_get_stream_in_sample_count(data_length);
    }

    return AUD_EXECUTION_SUCCESS;
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_send_amm
 * DESCRIPTION
 *  This function is used to send audio manager message.
 * PARAMETERS
 *  dest_id          [IN]
 *  src_id           [IN]
 *  cb_msg_id        [IN]
 *  msg_id           [IN]
 *  background_info  [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void bt_sink_srv_ami_send_amm(bt_sink_srv_am_module_t dest_id,
                              bt_sink_srv_am_module_t src_id,
                              bt_sink_srv_am_cb_msg_class_t cb_msg_id,
                              bt_sink_srv_am_msg_id_t msg_id,
                              bt_sink_srv_am_background_t *background_info,
                              uint8_t fromISR,
                              bt_sink_srv_am_amm_struct *pr_Amm)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
#ifndef WIN32_UT
    bt_sink_srv_am_amm_struct *g_prAmm_current = NULL;
#endif
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (!fromISR) {
#ifndef WIN32_UT
        g_prAmm_current = (bt_sink_srv_am_amm_struct *)pvPortMalloc(sizeof(bt_sink_srv_am_amm_struct));
#else
        g_prAmm_current = (bt_sink_srv_am_amm_struct *)malloc(sizeof(bt_sink_srv_am_amm_struct));
#endif
        g_prAmm_current->cb_msg_id = cb_msg_id;
        g_prAmm_current->src_mod_id = src_id;
        g_prAmm_current->dest_mod_id = dest_id;
        g_prAmm_current->msg_id = msg_id;
        memcpy(&(g_prAmm_current->background_info), background_info, sizeof(bt_sink_srv_am_background_t));

#ifndef WIN32_UT
        if (g_xQueue_am != 0) {
            xQueueSend(g_xQueue_am, (void *) &g_prAmm_current, portMAX_DELAY);
        }
#endif
    } else {
        pr_Amm->cb_msg_id = cb_msg_id;
        pr_Amm->src_mod_id = src_id;
        pr_Amm->dest_mod_id = dest_id;
        pr_Amm->msg_id = msg_id;
        memcpy(&(pr_Amm->background_info), background_info, sizeof(bt_sink_srv_am_background_t));

#ifndef WIN32_UT
        if (g_xQueue_am != 0) {
            xQueueSendFromISR(g_xQueue_am, (void *) &pr_Amm, &xHigherPriorityTaskWoken);
        }
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#endif
    }
}

#if STANDALONE_TEST
/* Hal API func */
hal_audio_status_t hal_audio_get_memory_size(uint32_t *memory_size)
{
    //*memory_size = 16<<10;
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_set_memory(uint16_t *memory)
{
    return HAL_AUDIO_STATUS_OK;
}
#endif
