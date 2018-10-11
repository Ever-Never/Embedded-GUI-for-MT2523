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

#include "aac_decoder_api.h"
#ifdef HAL_DVFS_MODULE_ENABLED
#include "hal_dvfs.h"
#define AAC_CPU_FREQ_L_BOUND (104000)
#endif /*HAL_DVFS_MODULE_ENABLED*/
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
#include "hal_audio_low_power.h"
#endif /*HAL_AUDIO_LOW_POWER_ENABLED*/
#ifdef MTK_AUDIO_AAC_DECODER_ENABLED
#include "aac_decoder_api_internal.h"
#include "memory_attribute.h"
#include "hal_cm4_topsm.h"

static aac_decoder_api_internal_handle_t *aac_decoder_api_internal_handle = NULL;
static QueueHandle_t aac_decoder_api_queue_handle = NULL;
static uint8_t aac_decoder_api_queue_reg_num = 0;
static aac_decoder_api_queue_event_id_t aac_decoder_api_queue_event_id_array[MAX_AAC_DECODER_FUNCTION];
static aac_decoder_api_internal_callback_t aac_decoder_api_queue_handler[MAX_AAC_DECODER_FUNCTION];
static TaskHandle_t aac_decoder_api_task_handle = NULL;
static volatile bool blIsRegister = false;
static volatile bool is_aac_exist = false;
void    aac_decoder_api_enter_suspend(void *data);
void    aac_decoder_api_enter_resume(void *data);

static void aac_decoder_api_event_send_from_isr(aac_decoder_api_queue_event_id_t id, void *parameter);
static aac_decoder_api_status_t aac_decoder_api_stop(aac_decoder_api_media_handle_t *handle);


#ifdef HAL_DVFS_MODULE_ENABLED
static bool aac_dvfs_valid(uint32_t voltage, uint32_t frequency)
{
    if (frequency < AAC_CPU_FREQ_L_BOUND) {
        return false;
    } else {
        return true;
    }
}

static dvfs_notification_t aac_dvfs_desc = {
    .domain = "VCORE",
    .module = "CM_CK0",
    .addressee = "aac_dvfs",
    .ops = {
        .valid = aac_dvfs_valid,
    }
};

static void aac_decoder_api_register_dsp_dvfs(bool flag)
{
    if (flag) {
        dvfs_register_notification(&aac_dvfs_desc);
        hal_dvfs_target_cpu_frequency(AAC_CPU_FREQ_L_BOUND, HAL_DVFS_FREQ_RELATION_L);
    } else {
        dvfs_deregister_notification(&aac_dvfs_desc);
    }

}
#endif /*HAL_DVFS_MODULE_ENABLED*/


static void pcm_out_isr_callback(hal_audio_event_t event, void *data)
{
    aac_decoder_api_internal_handle_t *internal_handle = aac_decoder_api_internal_handle;
    uint32_t consume = 0;
    uint32_t total_consume = 0;
    uint32_t sample_count = 0;
    uint32_t loop_idx = 0;
    uint32_t curr_cnt = 0;

    switch (event) {
        case HAL_AUDIO_EVENT_UNDERFLOW:
        case HAL_AUDIO_EVENT_DATA_REQUEST:
            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                uint8_t *pcm_out_buffer_ptr    = NULL;
                uint32_t pcm_out_buffer_data_count = 0;

                hal_audio_get_stream_out_sample_count(&sample_count);
                ring_buffer_get_read_information(&internal_handle->pcm_out_buffer, &pcm_out_buffer_ptr, &pcm_out_buffer_data_count);
                //LOGI("[ISR]loop_idx=%d pcm_count=%d sample_count=%d \r\n",loop_idx,pcm_out_buffer_data_count,sample_count);

                if (pcm_out_buffer_data_count > 0 && sample_count > 0) {
                    consume = MINIMUM(sample_count, pcm_out_buffer_data_count);
                    total_consume += consume;
                    hal_audio_write_stream_out(pcm_out_buffer_ptr, consume);
                    ring_buffer_read_done(&internal_handle->pcm_out_buffer, consume);
                    if ((sample_count - consume) == 0) {
                        break;
                    }
                } else {
                    hal_audio_write_stream_out(pcm_out_buffer_ptr, 0);
                    break;
                }
            }
            //LOGI("[ISR]total_consume=%d\r\n",total_consume);
            if (ring_buffer_get_space_byte_count(&internal_handle->pcm_out_buffer) >= internal_handle->pcm_buffer_size) {
                aac_decoder_api_event_send_from_isr(AAC_DECODER_API_QUEUE_EVENT_DECODE, NULL);
            }
            break;
        case HAL_AUDIO_EVENT_ERROR:
        case HAL_AUDIO_EVENT_NONE:
        case HAL_AUDIO_EVENT_DATA_NOTIFICATION:
            break;
    }

}

static hal_audio_channel_number_t translate_channel_number(uint32_t channel_number)
{
    hal_audio_channel_number_t hal_audio_channel_number = HAL_AUDIO_STEREO;

    hal_audio_channel_number = (channel_number == 1) ? HAL_AUDIO_MONO : HAL_AUDIO_STEREO;

    return hal_audio_channel_number;
}
static hal_audio_sampling_rate_t translate_sampling_rate(uint32_t sampling_rate)
{

    hal_audio_sampling_rate_t hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_44_1KHZ;

    switch (sampling_rate) {
        case 8000:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_8KHZ;
            break;
        case 11025:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_11_025KHZ;
            break;
        case 12000:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_12KHZ;
            break;
        case 16000:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_16KHZ;
            break;
        case 22050:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_22_05KHZ;
            break;
        case 24000:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_24KHZ;
            break;
        case 32000:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_32KHZ;
            break;
        case 44100:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_44_1KHZ;
            break;
        case 48000:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
            break;
    }
    return hal_audio_sampling_rate;
}


static void aac_decoder_api_set_share_buffer(aac_decoder_api_media_handle_t *handle, uint8_t *buffer, uint32_t length)
{
    handle->share_buffer.buffer_base = buffer;
    length &= ~0x1; // make buffer size even
    handle->share_buffer.buffer_size = length;
    handle->share_buffer.write = 0;
    handle->share_buffer.read = 0;
    handle->waiting = false;
    handle->underflow = false;
}

static void aac_decoder_api_get_write_buffer(aac_decoder_api_media_handle_t *handle, uint8_t **buffer, uint32_t *length)
{
    int32_t count = 0;

    if (handle->share_buffer.read > handle->share_buffer.write) {
        count = handle->share_buffer.read - handle->share_buffer.write - 1;
    } else if (handle->share_buffer.read == 0) {
        count = handle->share_buffer.buffer_size - handle->share_buffer.write - 1;
    } else {
        count = handle->share_buffer.buffer_size - handle->share_buffer.write;
    }
    *buffer = handle->share_buffer.buffer_base + handle->share_buffer.write;
    *length = count;
}


static void aac_decoder_api_get_read_buffer(aac_decoder_api_media_handle_t *handle, uint8_t **buffer, uint32_t *length)
{
    int32_t count = 0;

    if (handle->share_buffer.write >= handle->share_buffer.read) {
        count = handle->share_buffer.write - handle->share_buffer.read;
    } else {
        count = handle->share_buffer.buffer_size - handle->share_buffer.read;
    }
    *buffer = handle->share_buffer.buffer_base + handle->share_buffer.read;
    *length = count;
}


static void aac_decoder_api_write_data_done(aac_decoder_api_media_handle_t *handle, uint32_t length)
{
    handle->share_buffer.write += length;
    if (handle->share_buffer.write == handle->share_buffer.buffer_size) {
        handle->share_buffer.write = 0;
    }
}

static void aac_decoder_api_finish_write_data(aac_decoder_api_media_handle_t *handle)
{
    handle->waiting = false;
    handle->underflow = false;
}

static void aac_decoder_api_reset_share_buffer(aac_decoder_api_media_handle_t *handle)
{
    memset(handle->share_buffer.buffer_base, 0, handle->share_buffer.buffer_size);
    handle->share_buffer.write = 0;
    handle->share_buffer.read = 0;
    handle->waiting = false;
    handle->underflow = false;
}

static void aac_decoder_api_read_data_done(aac_decoder_api_media_handle_t *handle, uint32_t length)
{
    handle->share_buffer.read += length;
    if (handle->share_buffer.read == handle->share_buffer.buffer_size) {
        handle->share_buffer.read = 0;
    }
}


static int32_t aac_decoder_api_get_free_space(aac_decoder_api_media_handle_t *handle)
{
    int32_t count = 0;

    count = handle->share_buffer.read - handle->share_buffer.write - 2;
    if (count < 0) {
        count += handle->share_buffer.buffer_size;
    }
    return count;
}

static int32_t aac_decoder_api_get_data_count(aac_decoder_api_media_handle_t *handle)
{
    int32_t count = 0;

    count = handle->share_buffer.write - handle->share_buffer.read;
    if (count < 0) {
        count += handle->share_buffer.buffer_size;
    }
    return count;
}


static void aac_decoder_api_reset_pcm_out_buffer(void)
{
    aac_decoder_api_internal_handle_t *internal_handle = aac_decoder_api_internal_handle;
    internal_handle->pcm_out_buffer.read_pointer = 0;
    internal_handle->pcm_out_buffer.write_pointer = 0;
}


static void aac_decoder_api_buffer_function_init(aac_decoder_api_media_handle_t *handle)
{
    handle->set_share_buffer   = aac_decoder_api_set_share_buffer;
    handle->get_write_buffer   = aac_decoder_api_get_write_buffer;
    handle->get_read_buffer    = aac_decoder_api_get_read_buffer;
    handle->write_data_done    = aac_decoder_api_write_data_done;
    handle->finish_write_data  = aac_decoder_api_finish_write_data;
    handle->reset_share_buffer = aac_decoder_api_reset_share_buffer;
    handle->read_data_done     = aac_decoder_api_read_data_done;
    handle->get_free_space     = aac_decoder_api_get_free_space;
    handle->get_data_count     = aac_decoder_api_get_data_count;
}


static void aac_decoder_api_event_send_from_isr(aac_decoder_api_queue_event_id_t id, void *parameter)
{
    aac_decoder_api_queue_event_t event;
    event.id        = id;
    event.parameter = parameter;
    if (xQueueSendFromISR(aac_decoder_api_queue_handle, &event, 0) != pdPASS) {
        return;
    }
    return;
}


static void aac_decoder_api_event_register_callback(aac_decoder_api_queue_event_id_t reg_id, aac_decoder_api_internal_callback_t callback)
{
    uint32_t id_idx;
    for (id_idx = 0; id_idx < MAX_AAC_DECODER_FUNCTION; id_idx++) {
        if (aac_decoder_api_queue_event_id_array[id_idx] == AAC_DECODER_API_QUEUE_EVENT_NONE) {
            aac_decoder_api_queue_event_id_array[id_idx] = reg_id;
            aac_decoder_api_queue_handler[id_idx] = callback;
            aac_decoder_api_queue_reg_num++;
            break;
        }
    }
    return;
}


static void aac_decoder_api_event_deregister_callback(aac_decoder_api_queue_event_id_t dereg_id)
{
    LOGI("[CTRL]deregister HISR callback \n");
    uint32_t id_idx;
    for (id_idx = 0; id_idx < MAX_AAC_DECODER_FUNCTION; id_idx++) {
        if (aac_decoder_api_queue_event_id_array[id_idx] == dereg_id) {
            aac_decoder_api_queue_event_id_array[id_idx] = AAC_DECODER_API_QUEUE_EVENT_NONE;
            aac_decoder_api_queue_reg_num--;
            break;
        }
    }
    return;
}

static void aac_decoder_api_request_data(void)
{
    aac_decoder_api_internal_handle_t *internal_handle = aac_decoder_api_internal_handle;
    aac_decoder_api_media_handle_t *handle = &internal_handle->handle;

    if (!internal_handle->flush) {
        if (!handle->waiting) {
            handle->waiting = true;
            handle->handler(handle, AAC_DECODER_API_MEDIA_EVENT_REQUEST);
        }
    }
}

static void aac_decoder_api_task_main(void *arg)
{
    aac_decoder_api_queue_event_t event;

    while (1) {
        if (xQueueReceive(aac_decoder_api_queue_handle, &event, portMAX_DELAY)) {
            aac_decoder_api_queue_event_id_t rece_id = event.id;
            uint8_t id_idx;
            for (id_idx = 0; id_idx < MAX_AAC_DECODER_FUNCTION; id_idx++) {
                if (aac_decoder_api_queue_event_id_array[id_idx] == rece_id) {
                    aac_decoder_api_queue_handler[id_idx](event.parameter);
                    break;
                }
            }
        }
    }
}


static void aac_decoder_api_task_create(void)
{
    if (aac_decoder_api_task_handle ==  NULL) {
        LOGI("create aac decoder task\r\n");
        xTaskCreate(aac_decoder_api_task_main, AAC_DECODER_API_TASK_NAME, AAC_DECODER_API_TASK_STACKSIZE / sizeof(StackType_t), NULL, AAC_DECODER_API_TASK_PRIO, &aac_decoder_api_task_handle);
    }
}


static void  aac_decoder_api_decode_hisr(void *data)
{
    aac_decoder_api_internal_handle_t *internal_handle = aac_decoder_api_internal_handle;
    aac_decoder_api_media_handle_t *handle = &internal_handle->handle;

    uint32_t share_buffer_data_amount = 0;
    uint32_t share_buffer_data = 0;
    uint8_t *share_buffer_read_ptr = NULL;
    uint8_t *P_dst = NULL;
    uint8_t *P_src = NULL;
    uint32_t remain = 0;
    uint32_t consume = 0;

    uint32_t check_size        = SHARE_BUFFER_MINIMUM_SIZE;
    uint32_t read_buf_size     = internal_handle->read_buffer_size; //must use internal_handle->read_buffer_size for read_buf_size in hisr
    uint32_t read_buf_max_size = internal_handle->bitstream_buffer_size;
    uint32_t remained_buf_size = internal_handle->remained_buf_size;
    uint32_t input_buf_size    = internal_handle->bitstream_buffer_size;
    uint32_t output_buf_size   = internal_handle->pcm_buffer_size;
    uint8_t *P_in_buf = (uint8_t *)internal_handle->bitstream_buffer;
    uint8_t *P_ou_buf = (uint8_t *)internal_handle->pcm_buffer;

    if ((ring_buffer_get_space_byte_count(&internal_handle->pcm_out_buffer)) < internal_handle->pcm_buffer_size) {
        LOGI("[HISR]Pcm out buffer is full\r\n");
        return ;
    }

    share_buffer_data_amount = handle->get_data_count(handle);
    if (share_buffer_data_amount < SHARE_BUFFER_CHECK_SIZE) {
        aac_decoder_api_request_data();
    }

    //Check available data
    share_buffer_data_amount = handle->get_data_count(handle);
    if (share_buffer_data_amount > 0) {
        remain = read_buf_max_size - remained_buf_size;
        for (uint32_t loop_idx = 0; loop_idx < 2; loop_idx++) {
            if (remain == 0) {
                break;
            }
            handle->get_read_buffer(handle, &share_buffer_read_ptr, &share_buffer_data);
            if (share_buffer_data > 0) {
                consume = MINIMUM(share_buffer_data, remain);
                memcpy(&P_in_buf[remained_buf_size], share_buffer_read_ptr, consume);
                handle->read_data_done(handle, consume);
                remain -= consume;
                remained_buf_size += consume;
            }
        }
    }

    if (remained_buf_size == 0) {
        return;
    }

    int32_t result;
#ifdef MTK_AUDIO_AAC_DECODER_FEATURE_HEAAC
    memset(&internal_handle->info, 0, sizeof(heaac_decoder_runtime_information_t));
    result = heaac_decoder_process(internal_handle->dec_handle, internal_handle->temp_buffer, P_in_buf, &input_buf_size, (int16_t *)P_ou_buf, &output_buf_size, &internal_handle->info);
#else
    result = aac_decoder_process(internal_handle->dec_handle, internal_handle->temp_buffer, P_in_buf, &input_buf_size, (int16_t *)P_ou_buf, &output_buf_size, &internal_handle->info);
#endif
    if (result < 0) {
        LOGW("[HISR]DECODER ERROR(%d), remained_buf_size=%d\r\n", (int)result, (unsigned int)remained_buf_size);
        return AAC_DECODER_API_STATUS_ERROR;
    }

    //LOGI(" Update consumed & remained buffer size\n");
    {   /* Update consumed & remained buffer size */
        read_buf_size = input_buf_size;
        if (remained_buf_size > input_buf_size) {
            remained_buf_size -= input_buf_size;
            memmove(P_in_buf, &P_in_buf[read_buf_size], remained_buf_size);
        } else {
            remained_buf_size = 0;
        }
        internal_handle->read_buffer_size = read_buf_size;
        internal_handle->remained_buf_size = remained_buf_size;
    }

    if (result >= 0) {
        {   /* Write deocded pcm to output buffer */
            remain = output_buf_size;
            uint32_t pcm_out_buffer_free_space = 0;
            P_src = (uint8_t *)P_ou_buf;
            for (uint32_t loop_idx = 0; loop_idx < 2; loop_idx++) {
                if (remain == 0) {
                    break;
                }
                ring_buffer_get_write_information(&internal_handle->pcm_out_buffer, &P_dst, &pcm_out_buffer_free_space);
                if (pcm_out_buffer_free_space > 0) {
                    consume = MINIMUM(pcm_out_buffer_free_space, remain);
                    memcpy(P_dst, P_src, consume);
                    ring_buffer_write_done(&internal_handle->pcm_out_buffer, consume);
                    P_src += consume;
                    remain -= consume;
                }
            }

            if (remain != 0) {
                LOGW("stream_out_pcm_buff space not enough for 1 frame, remain=%d!\r\n", remain);
            }
        }

    }

    if (ring_buffer_get_space_byte_count(&internal_handle->pcm_out_buffer) >= internal_handle->pcm_buffer_size) {
        aac_decoder_api_event_send_from_isr(AAC_DECODER_API_QUEUE_EVENT_DECODE, NULL);
    }

    return AAC_DECODER_API_STATUS_OK;

}


static aac_decoder_api_status_t aac_decoder_api_decode(void)
{
    LOGI("Decoding\r\n");
    aac_decoder_api_internal_handle_t *internal_handle = aac_decoder_api_internal_handle;
    aac_decoder_api_media_handle_t *handle = &internal_handle->handle;

    /*rinf buffer use*/
    uint32_t share_buffer_data_amount = 0;
    uint32_t share_buffer_data = 0;
    uint8_t *share_buffer_read_ptr = NULL;
    uint8_t *P_dst = NULL;
    uint8_t *P_src = NULL;
    uint32_t remain = 0;
    uint32_t consume = 0;

    uint32_t read_buf_size     = internal_handle->bitstream_buffer_size;// Data size in bitstream buffer consumed by decoder
    uint32_t read_buf_max_size = internal_handle->bitstream_buffer_size;
    uint32_t remained_buf_size = 0;                                     // Data  in bitstream buffer decoder can read
    uint8_t *P_in_buf = (uint8_t *)internal_handle->bitstream_buffer;
    uint8_t *P_ou_buf = (uint8_t *)internal_handle->pcm_buffer;

    internal_handle->frame = 0;

    while (1) {
        if ((ring_buffer_get_space_byte_count(&internal_handle->pcm_out_buffer)) < internal_handle->pcm_buffer_size) {
            LOGI("[PLAY]Pcm out buffer is full\r\n");
            return AAC_DECODER_API_STATUS_OK;;
        }
        uint32_t  input_buf_size  = internal_handle->bitstream_buffer_size;
        uint32_t  output_buf_size = internal_handle->pcm_buffer_size;

        //check available data
        share_buffer_data_amount = handle->get_data_count(handle);
        if (share_buffer_data_amount > 0) {
            remain = read_buf_max_size - remained_buf_size;
            for (uint32_t loop_idx = 0; loop_idx < 2; loop_idx++) {
                if (remain == 0) {
                    break;
                }
                handle->get_read_buffer(handle, &share_buffer_read_ptr, &share_buffer_data);
                if (share_buffer_data > 0) {
                    consume = MINIMUM(share_buffer_data, remain);
                    memcpy(&P_in_buf[remained_buf_size], share_buffer_read_ptr, consume);
                    handle->read_data_done(handle, consume);
                    remain -= consume;
                    remained_buf_size += consume;
                }
            }
        }

        if (remained_buf_size == 0) {
            LOGI("[PLAY]remained_buf_size=%d, break loop\r\n", (unsigned int)remained_buf_size);
            break;
        }

        int32_t result;
        //memset(internal_handle->temp_buffer, 0, internal_handle->temp_buffer_size);
#ifdef MTK_AUDIO_AAC_DECODER_FEATURE_HEAAC
        memset(&internal_handle->info, 0, sizeof(heaac_decoder_runtime_information_t));
        result = heaac_decoder_process(internal_handle->dec_handle, internal_handle->temp_buffer, P_in_buf, &input_buf_size, (int16_t *)P_ou_buf, &output_buf_size, &internal_handle->info);
#else
        result = aac_decoder_process(internal_handle->dec_handle, internal_handle->temp_buffer, P_in_buf, &input_buf_size, (int16_t *)P_ou_buf, &output_buf_size, &internal_handle->info);
#endif
        if (result < 0) {
            LOGW("[PLAY]DECODER ERROR(%d)\r\n", (int)result);
            break;//todo
        }

        {   /* Update consumed & remained buffer size */
            read_buf_size = input_buf_size;
            if (remained_buf_size > input_buf_size) {
                remained_buf_size -= input_buf_size;
                memmove(P_in_buf, &P_in_buf[read_buf_size], remained_buf_size);
            } else {
                remained_buf_size = 0;
            }
            internal_handle->read_buffer_size = read_buf_size;
            internal_handle->remained_buf_size = remained_buf_size;
        }

        if (result >= 0) {
            {
                // Write deocded pcm to output buffer
                remain = output_buf_size;
                uint32_t pcm_out_buffer_free_space = 0;
                P_src = P_ou_buf;
                for (uint32_t loop_idx = 0; loop_idx < 2; loop_idx++) {
                    if (remain == 0) {
                        break;
                    }
                    ring_buffer_get_write_information(&internal_handle->pcm_out_buffer, &P_dst, &pcm_out_buffer_free_space);
                    if (pcm_out_buffer_free_space > 0) {
                        consume = MINIMUM(pcm_out_buffer_free_space, remain);
                        memcpy(P_dst, P_src, consume);
                        ring_buffer_write_done(&internal_handle->pcm_out_buffer, consume);
                        P_src += consume;
                        remain -= consume;
                    }
                }

                if (remain != 0) {
                    LOGW("[PLAY]stream_out_pcm_buff space not enough for 1 frame, remain=%d!\n", remain);
                }
            }
        }

    }

    return AAC_DECODER_API_STATUS_OK;

}

static aac_decoder_api_status_t aac_decoder_api_play(aac_decoder_api_media_handle_t *handle)
{
    LOGI("[CTRL]PLAY\r\n");

    if (handle->state != AAC_DECODER_API_STATE_READY && handle->state != AAC_DECODER_API_STATE_STOP) {
        LOGE("DECODER state ERROR (%d)\r\n", handle->state);
        return AAC_DECODER_API_STATUS_ERROR;
    }
    aac_decoder_api_internal_handle_t *internal_handle = (aac_decoder_api_internal_handle_t *) handle;
    aac_decoder_api_status_t api_status = AAC_DECODER_API_STATUS_OK;
    uint32_t audio_memory_size = 0;//pcm playback use
    uint32_t share_buffer_data_amount = 0;
    int32_t result = 0;

    aac_decoder_api_event_register_callback(AAC_DECODER_API_QUEUE_EVENT_DECODE, aac_decoder_api_decode_hisr);

    internal_handle->first_play = true;
    aac_decoder_api_reset_pcm_out_buffer();
    aac_decoder_api_request_data();

    //Initailize decoder and do first decoding
#ifdef MTK_AUDIO_AAC_DECODER_FEATURE_HEAAC
    LOGI("Init HEAAC DECODER\r\n");
    result = heaac_decoder_init(&internal_handle->dec_handle, internal_handle->internal_buffer, &internal_handle->init_param);
    if (result < 0) {
        LOGE("Init HEAAC DECODER ERROR (%d)\n", (int)result);
        return AAC_DECODER_API_STATUS_ERROR;
    }
#else
    LOGI("Init AAC DECODER\r\n");
    result = aac_decoder_init(&internal_handle->dec_handle, internal_handle->internal_buffer);
    if (result < 0) {
        LOGE("Init AAC DECODER ERROR (%d)\n", (int)result);
        return AAC_DECODER_API_STATUS_ERROR;
    }
#endif

    api_status = aac_decoder_api_decode();
    if (api_status != AAC_DECODER_API_STATUS_OK) {
        LOGE("DECODER ERROR\n");
        return AAC_DECODER_API_STATUS_ERROR;
    }
    LOGI("[PLAY]info.sampling_rate=%d\r\n", (unsigned int)internal_handle->info.sampling_rate);
    LOGI("[PLAY]info.frame_size=%d\r\n", (unsigned int)internal_handle->info.frame_size);
    LOGI("[PLAY]info.channel_number=%d\r\n", (unsigned int)internal_handle->info.channel_number);
    LOGI("[PLAY]info.sbr_flag=%d\r\n", (unsigned int)internal_handle->info.sbr_flag);

    hal_audio_set_stream_out_sampling_rate(translate_sampling_rate(internal_handle->info.sampling_rate));
    hal_audio_set_stream_out_channel_number(translate_channel_number(internal_handle->info.channel_number));
    hal_audio_register_stream_out_callback(pcm_out_isr_callback, NULL);
    hal_audio_get_memory_size(&audio_memory_size);
    internal_handle->audio_buffer = (uint8_t *)pvPortMalloc(audio_memory_size);
    if (NULL == internal_handle->audio_buffer) {
        LOGE("Alloc audio_buffer(%d) failed!!! \n", (unsigned int)audio_memory_size);
        return AAC_DECODER_API_STATUS_ERROR;
    } else {
        LOGI("Alloc audio_buffer(%d) successfully \n", (unsigned int)audio_memory_size);
    }

    hal_audio_set_memory(internal_handle->audio_buffer);
    hal_audio_start_stream_out(HAL_AUDIO_PLAYBACK_MUSIC);
    aac_decoder_api_event_send_from_isr(AAC_DECODER_API_QUEUE_EVENT_DECODE, NULL);

    handle->state = AAC_DECODER_API_STATE_PLAY;

    return AAC_DECODER_API_STATUS_OK;

}


static aac_decoder_api_status_t aac_decoder_api_stop(aac_decoder_api_media_handle_t *handle)
{
    LOGI("[CTRL]STOP\r\n");

    if (handle->state != AAC_DECODER_API_STATE_PLAY && handle->state != AAC_DECODER_API_STATE_PAUSE
            && handle->state != AAC_DECODER_API_STATE_RESUME) {
        LOGI("DECODER state error (%d)\r\n", handle->state);
        return AAC_DECODER_API_STATUS_ERROR;
    }

    aac_decoder_api_internal_handle_t *internal_handle = (aac_decoder_api_internal_handle_t *) handle;

    hal_audio_stop_stream_out();
    aac_decoder_api_event_deregister_callback(AAC_DECODER_API_QUEUE_EVENT_DECODE);
    aac_decoder_api_reset_share_buffer(handle);
    aac_decoder_api_reset_pcm_out_buffer();

    if (internal_handle->audio_buffer) {
        vPortFree(internal_handle->audio_buffer);
    }

    handle->state = AAC_DECODER_API_STATE_STOP;

    return AAC_DECODER_API_STATUS_OK;
}

static aac_decoder_api_status_t aac_decoder_api_flush(aac_decoder_api_media_handle_t *handle, int32_t flush_data_flag)
{
    LOGI("[CTRL]FLUSH\n");

    aac_decoder_api_internal_handle_t *internal_handle = (aac_decoder_api_internal_handle_t *) handle;

    internal_handle->flush = (flush_data_flag == 1) ? true : false;

    return AAC_DECODER_API_STATUS_OK;
}

static aac_decoder_api_status_t aac_decoder_api_pause(aac_decoder_api_media_handle_t *handle)
{
    LOGI("[CTRL]PAUSE\n");

    if (handle->state != AAC_DECODER_API_STATE_PLAY) {
        return AAC_DECODER_API_STATUS_ERROR;
    }

    hal_audio_stop_stream_out();

    handle->state = AAC_DECODER_API_STATE_PAUSE;
    return AAC_DECODER_API_STATUS_OK;
}

static aac_decoder_api_status_t aac_decoder_api_resume(aac_decoder_api_media_handle_t *handle)
{
    LOGI("[CTRL]RESUME\n");

    if (handle->state != AAC_DECODER_API_STATE_PAUSE) {
        return AAC_DECODER_API_STATUS_ERROR;
    }

    hal_audio_start_stream_out(HAL_AUDIO_PLAYBACK_MUSIC);

    handle->state = AAC_DECODER_API_STATE_RESUME;

    return AAC_DECODER_API_STATUS_OK;
}


static aac_decoder_api_status_t aac_decoder_api_process(aac_decoder_api_media_handle_t *handle, aac_decoder_api_media_event_t event)
{
    LOGI("[CTRL]PROCESS\n");

    aac_decoder_api_internal_handle_t *internal_handle = (aac_decoder_api_internal_handle_t *) handle;
    if (internal_handle == NULL) {
        return AAC_DECODER_API_STATUS_ERROR;
    }
    return AAC_DECODER_API_STATUS_OK;
}

aac_decoder_api_media_handle_t *aac_deocder_api_open(aac_decoder_api_callback_t callback)
{
    LOGI("[CTRL]OPEN\r\n");
    aac_decoder_api_media_handle_t *handle;
    aac_decoder_api_internal_handle_t *internal_handle; /*internal handler*/
    int32_t result = 0;

    internal_handle = (aac_decoder_api_internal_handle_t *)pvPortMalloc(sizeof(aac_decoder_api_internal_handle_t));
    if (internal_handle == NULL) {
        LOGE("Alloc internal_handle failed\r\n");
        return NULL;
    }
    memset(internal_handle, 0, sizeof(aac_decoder_api_internal_handle_t));
    aac_decoder_api_internal_handle = internal_handle;
    internal_handle->flush = false;

    handle = &internal_handle->handle;
    handle->audio_id = 0xbeef;
    handle->handler = callback;
    handle->play    = aac_decoder_api_play;
    handle->stop    = aac_decoder_api_stop;
    handle->pause   = aac_decoder_api_pause;
    handle->resume  = aac_decoder_api_resume;
    handle->process = aac_decoder_api_process;
    handle->flush   = aac_decoder_api_flush;
    aac_decoder_api_buffer_function_init(handle);

    aac_decoder_api_queue_handle = xQueueCreate(AAC_DECODER_QUEUE_SIZE, sizeof(aac_decoder_api_queue_event_t));
    /* Initialize queue registration */
    uint8_t id_idx;
    for (id_idx = 0; id_idx < MAX_AAC_DECODER_FUNCTION; id_idx++) {
        aac_decoder_api_queue_event_id_array[id_idx] = AAC_DECODER_API_QUEUE_EVENT_NONE;
    }

#ifdef MTK_AUDIO_AAC_DECODER_FEATURE_HEAAC
    heaac_decoder_buffer_size_t mem_size;
    memset(&mem_size, 0, sizeof(heaac_decoder_buffer_size_t));
    result = heaac_decoder_get_buffer_size(&mem_size);
    if (result < 0) {
        LOGE("Get buffer size for HEAAC ERROR (%d)\r\n", result);
        return NULL;
    }

    internal_handle->init_param.sbr_signaling     = HEAAC_DECODER_SBR_SIGNALING_AUTO;
    internal_handle->init_param.sbr_mode          = HEAAC_DECODER_SBR_MODE_AUTO;
    internal_handle->init_param.force_stereo_flag = 0;
    is_aac_exist = true;
#else
    aac_decoder_buffer_size_t mem_size;
    memset(&mem_size, 0, sizeof(aac_decoder_buffer_size_t));
    result = aac_decoder_get_buffer_size(&mem_size);
    if (result < 0) {
        LOGE("Get buffer size for AAC ERROR (%d)\r\n", (int)result);
        return NULL;
    }
#endif

    internal_handle->internal_buffer_size = mem_size.internal_buffer_size;
    internal_handle->temp_buffer_size = mem_size.temporary_buffer_size;
    internal_handle->pcm_buffer_size = mem_size.pcm_buffer_size;
    internal_handle->bitstream_buffer_size = mem_size.bitstream_buffer_size;
    internal_handle->pcm_out_buffer_size = mem_size.pcm_buffer_size * PRE_BUFF_PCM_MULTIPLE; //HEAAC=8192*3, AAC LC=4096*3

    //alloc decoder internal buffer
    internal_handle->internal_buffer = (uint8_t *)pvPortMalloc(internal_handle->internal_buffer_size);
    if ((internal_handle->internal_buffer == NULL)) {
        LOGE("Alloc internal_buffer failed\r\n");
        return NULL;
    }

    internal_handle->temp_buffer = (uint8_t *)pvPortMalloc(internal_handle->temp_buffer_size);
    if ((internal_handle->temp_buffer == NULL)) {
        LOGE("Alloc temp_buffer failed\r\n");
        return NULL;
    }

    internal_handle->pcm_buffer = (int16_t *)pvPortMalloc(internal_handle->pcm_buffer_size);
    if ((internal_handle->pcm_buffer == NULL)) {
        LOGE("alloc pcm_buffer failed\r\n");
        return NULL;
    }

    internal_handle->bitstream_buffer = (uint8_t *)pvPortMalloc(internal_handle->bitstream_buffer_size);
    if ((internal_handle->bitstream_buffer == NULL)) {
        LOGE("Alloc bitstream_buffer failed\r\n");
        return NULL;
    }

    //alloc pcm out buffer
    internal_handle->pcm_out_buffer.buffer_byte_count = internal_handle->pcm_out_buffer_size;
    internal_handle->pcm_out_buffer.buffer_base_pointer = (uint8_t *)pvPortMalloc(internal_handle->pcm_out_buffer.buffer_byte_count);
    internal_handle->pcm_out_buffer.read_pointer = 0;
    internal_handle->pcm_out_buffer.write_pointer = 0;
    if ((internal_handle->pcm_out_buffer.buffer_base_pointer == NULL)) {
        LOGE("Alloc pcm_out_buffer failed\r\n");
        return NULL;
    }

    aac_decoder_api_task_create();
    if (blIsRegister == false) {
        blIsRegister = true;
        hal_cm4_topsm_register_resume_cb((cm4_topsm_cb)aac_decoder_api_enter_resume, NULL);
        hal_cm4_topsm_register_suspend_cb((cm4_topsm_cb)aac_decoder_api_enter_suspend, NULL);
    }
    handle->state = AAC_DECODER_API_STATE_READY;
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
    audio_lowpower_set_mode(true);
#endif

    return handle;

}


aac_decoder_api_status_t aac_decoder_api_close(aac_decoder_api_media_handle_t *handle)
{
    LOGI("[CTRL]CLOSE AAC DECODER\r\n");

    aac_decoder_api_internal_handle_t *internal_handle = (aac_decoder_api_internal_handle_t *) handle;

    if (handle->state != AAC_DECODER_API_STATE_STOP && handle->state != AAC_DECODER_API_STATE_READY) {
        return AAC_DECODER_API_STATUS_ERROR;
    }

    handle->state = AAC_DECODER_API_STATE_IDLE;
    is_aac_exist = false;

    if (aac_decoder_api_queue_handle != NULL) {
        vQueueDelete(aac_decoder_api_queue_handle);
        aac_decoder_api_queue_handle = NULL;
    }
    if (aac_decoder_api_task_handle != NULL) {
        vTaskDelete(aac_decoder_api_task_handle);
        aac_decoder_api_task_handle = NULL;
    }
    //release decoder buffer
    if (internal_handle->bitstream_buffer) {
        vPortFree(internal_handle->bitstream_buffer);
        internal_handle->bitstream_buffer = NULL;
    }

    if (internal_handle->internal_buffer) {
        vPortFree(internal_handle->internal_buffer);
        internal_handle->internal_buffer = NULL;
    }

    if (internal_handle->temp_buffer) {
        vPortFree(internal_handle->temp_buffer);
        internal_handle->temp_buffer = NULL;
    }

    if (internal_handle->pcm_buffer) {
        vPortFree(internal_handle->pcm_buffer);
        internal_handle->pcm_buffer = NULL;
    }

    //release pcm out buffer
    if (internal_handle->pcm_out_buffer.buffer_base_pointer) {
        vPortFree(internal_handle->pcm_out_buffer.buffer_base_pointer);
        internal_handle->pcm_out_buffer.buffer_base_pointer = NULL;
    }

    //release internal handle
    if (internal_handle) {
        vPortFree(internal_handle);
        internal_handle = NULL;
    }
    aac_decoder_api_internal_handle = NULL;

#ifdef HAL_AUDIO_LOW_POWER_ENABLED
    audio_lowpower_set_mode(false);
#endif

    return AAC_DECODER_API_STATUS_OK;

}

void aac_decoder_api_enter_resume(void *data)
{
    aac_decoder_api_internal_handle_t *internal_handle = aac_decoder_api_internal_handle;
    if (is_aac_exist == false) {
        return ;
    }

#ifdef HAL_DVFS_MODULE_ENABLED
    aac_decoder_api_register_dsp_dvfs(true);
#endif

}


void aac_decoder_api_enter_suspend(void *data)
{
    aac_decoder_api_internal_handle_t *internal_handle = aac_decoder_api_internal_handle;
    if (is_aac_exist == false) {
        return ;
    }

#ifdef HAL_DVFS_MODULE_ENABLED
    aac_decoder_api_register_dsp_dvfs(false);
#endif
}

#endif /*MTK_AUDIO_AAC_DECODER_ENABLED*/


