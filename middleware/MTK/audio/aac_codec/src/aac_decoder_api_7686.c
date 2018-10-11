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
#ifdef MTK_AUDIO_AAC_DECODER_ENABLED
#include "aac_decoder_api_internal_7686.h"
#include "memory_attribute.h"
#include "hal_i2s.h"
#include "nau8810.h"
#include "hal_i2c_master.h"

#define I2S_TX_BUFFER_LENGTH 4096

static aac_decoder_api_internal_handle_t *aac_decoder_api_internal_handle = NULL;
static QueueHandle_t aac_decoder_api_queue_handle = NULL;
static uint8_t aac_decoder_api_queue_reg_num = 0;
static aac_decoder_api_queue_event_id_t aac_decoder_api_queue_event_id_array[MAX_AAC_DECODER_FUNCTION];
static aac_decoder_api_internal_callback_t aac_decoder_api_queue_handler[MAX_AAC_DECODER_FUNCTION];
static TaskHandle_t aac_decoder_api_task_handle = NULL;
static volatile bool bl_is_register_sleep_cb = false;
static hal_i2s_port_t i2s_port = HAL_I2S_1;
static uint32_t *i2s_tx_buffer = NULL;
static volatile bool is_aac_exist = false;

void aac_decoder_api_enter_suspend(void *data);
void aac_decoder_api_enter_resume(void *data);

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

/*
static hal_i2s_channel_number_t translate_channel_number(uint32_t channel_number)
{
    hal_i2s_channel_number_t i2s_channel_number = HAL_I2S_STEREO;

    i2s_channel_number = (channel_number == 1) ? HAL_I2S_MONO : HAL_I2S_STEREO;

    return i2s_channel_number;
}
*/

static hal_i2s_sample_rate_t translate_sampling_rate(uint32_t sampling_rate)
{

    hal_i2s_sample_rate_t i2s_sample_rate = HAL_I2S_SAMPLE_RATE_8K;

    switch (sampling_rate) {
        case 8000:
            i2s_sample_rate = HAL_I2S_SAMPLE_RATE_8K;
            break;
        case 11025:
            i2s_sample_rate = HAL_I2S_SAMPLE_RATE_11_025K;
            break;
        case 12000:
            i2s_sample_rate = HAL_I2S_SAMPLE_RATE_12K;
            break;
        case 16000:
            i2s_sample_rate = HAL_I2S_SAMPLE_RATE_16K;
            break;
        case 22050:
            i2s_sample_rate = HAL_I2S_SAMPLE_RATE_22_05K;
            break;
        case 24000:
            i2s_sample_rate = HAL_I2S_SAMPLE_RATE_24K;
            break;
        case 32000:
            i2s_sample_rate = HAL_I2S_SAMPLE_RATE_32K;
            break;
        case 44100:
            i2s_sample_rate = HAL_I2S_SAMPLE_RATE_44_1K;
            break;
        case 48000:
            i2s_sample_rate = HAL_I2S_SAMPLE_RATE_48K;
            break;
    }
    return i2s_sample_rate;
}

static AUCODEC_SAMPLERATE_SEL_e translate_8810_sampling_rate(hal_i2s_sample_rate_t i2s_sample_rate)
{

    AUCODEC_SAMPLERATE_SEL_e SAMPLERATE_SEL = eSR48KHz;

    switch (i2s_sample_rate) {
        case HAL_I2S_SAMPLE_RATE_8K:
            SAMPLERATE_SEL = eSR8KHz;
            break;
        case HAL_I2S_SAMPLE_RATE_11_025K:
            SAMPLERATE_SEL = eSR11_025KHz;
            break;
        case HAL_I2S_SAMPLE_RATE_12K:
            SAMPLERATE_SEL = eSR12KHz;
            break;
        case HAL_I2S_SAMPLE_RATE_16K:
            SAMPLERATE_SEL = eSR16KHz;
            break;
        case HAL_I2S_SAMPLE_RATE_22_05K:
            SAMPLERATE_SEL = eSR22_05KHz;
            break;
        case HAL_I2S_SAMPLE_RATE_24K:
            SAMPLERATE_SEL = eSR24KHz;
            break;
        case HAL_I2S_SAMPLE_RATE_32K:
            SAMPLERATE_SEL = eSR32KHz;
            break;
        case HAL_I2S_SAMPLE_RATE_44_1K:
            SAMPLERATE_SEL = eSR44K1Hz;
            break;
        case HAL_I2S_SAMPLE_RATE_48K:
            SAMPLERATE_SEL = eSR48KHz;
            break;
    }
    return SAMPLERATE_SEL;
}


static void i2s_tx_callback(hal_i2s_event_t event, void *data)
{
    hal_i2s_disable_tx_dma_interrupt_ex(i2s_port);

    aac_decoder_api_internal_handle_t *internal_handle = aac_decoder_api_internal_handle;
    uint32_t loop_idx = 0;
    uint32_t consume_samples = 0; //in sample , not in byte
    uint32_t total_consume_samples = 0;
    uint32_t tx_space_in_sample = 0; //in samples (vfifo is 4bytes), not in byte
    uint32_t i2s_data = 0;
    //uint32_t i2s_last_data = 0;
    uint32_t step = 0;

    switch (event) {
        case HAL_I2S_EVENT_DATA_REQUEST:
            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                uint8_t *pcm_out_buffer_ptr    = NULL;
                uint32_t pcm_out_buffer_data_count = 0;//in bytes

                hal_i2s_get_tx_sample_count_ex(i2s_port, &tx_space_in_sample);
                ring_buffer_get_read_information(&internal_handle->pcm_out_buffer, &pcm_out_buffer_ptr, &pcm_out_buffer_data_count);
                pcm_out_buffer_data_count &= ~0x1; // make it even
                if (internal_handle->info.channel_number == 1) { //mono
                    pcm_out_buffer_data_count >>= 1; // 2bytes for mono
                    step = 2;
                }
                if (internal_handle->info.channel_number == 2) { //stereo
                    pcm_out_buffer_data_count >>= 2; // 4bytes for mono
                    step = 4;
                }
                if ((tx_space_in_sample > 0) && (pcm_out_buffer_data_count > 0)) {
                    consume_samples = MINIMUM(tx_space_in_sample, pcm_out_buffer_data_count);
                    //LOGI("[ISR]consume_samples=%d \r\n",consume_samples);
                    for (uint32_t i = 0; i < consume_samples; i++) {
                        i2s_data = 0;
                        memcpy(&i2s_data, pcm_out_buffer_ptr, step);
                        hal_i2s_tx_write_ex(i2s_port, i2s_data);
                        //LOGI("i=%d i2s_data=%x \r\n",i, i2s_data);
                        //i2s_last_data = i2s_data;
                        pcm_out_buffer_ptr += step;
                    }
                    total_consume_samples += consume_samples;
                    ring_buffer_read_done(&internal_handle->pcm_out_buffer, consume_samples * step);
                }
            }
            //LOGI("[ISR]total_consume_samples=%d\r\n",total_consume_samples);
            if (ring_buffer_get_space_byte_count(&internal_handle->pcm_out_buffer) >= internal_handle->pcm_buffer_size) {
                aac_decoder_api_event_send_from_isr(AAC_DECODER_API_QUEUE_EVENT_DECODE, NULL);
            }
            break;
    }
    hal_i2s_enable_tx_dma_interrupt_ex(i2s_port);

}


static void i2s_tx_enable(void)
{
    LOGI("[CTRL]I2S TX ENABLE\r\n");
    hal_i2s_register_tx_vfifo_callback_ex(i2s_port, i2s_tx_callback, NULL);
    hal_i2s_enable_tx_dma_interrupt_ex(i2s_port);
    hal_i2s_enable_tx_ex(i2s_port);
    hal_i2s_enable_audio_top_ex(i2s_port);
}


static void i2s_tx_disable(void)
{
    LOGI("[CTRL]I2S TX DISABLE\r\n");
    hal_i2s_disable_tx_dma_interrupt_ex(i2s_port);
    hal_i2s_disable_tx_ex(i2s_port);
    hal_i2s_disable_audio_top_ex(i2s_port);
    hal_i2s_deinit_ex(i2s_port);
}

static aac_decoder_api_status_t nau8810_configure(AUCODEC_SAMPLERATE_SEL_e aac_sampling_rate)
{
    LOGI("[CTRL]NAU8810 OPEN \r\n");

    /*configure NAU8810*/
    AUCODEC_STATUS_e codec_status;
    hal_i2c_port_t i2c_port;
    hal_i2c_frequency_t frequency;

    codec_status = AUCODEC_STATUS_OK;
    i2c_port = HAL_I2C_MASTER_1;
    frequency = HAL_I2C_FREQUENCY_50K;

    codec_status = aucodec_i2c_init(i2c_port, frequency);
    if (codec_status != AUCODEC_STATUS_OK) {
        LOGE("aucodec_i2c_init failed\r\n");
    }

    aucodec_softreset();//soft reset

    codec_status = aucodec_init();
    if (codec_status != AUCODEC_STATUS_OK) {
        LOGE("aucodec_init failed\r\n");
    }
    aucodec_set_dai_fmt(eI2S, e16Bit, eBCLK_NO_INV);

    switch (aac_sampling_rate) {
        case eSR48KHz:
        case eSR32KHz:
        case eSR16KHz:
        case eSR8KHz:
        case eSR24KHz:
        case eSR12KHz:
            codec_status = aucodec_set_dai_sysclk(aac_sampling_rate, eSLAVE, e32xFS, eMCLK8KBASE, ePLLEnable);
            break;

        case eSR44K1Hz:
        case eSR22_05KHz:
        case eSR11_025KHz:
            codec_status = aucodec_set_dai_sysclk(aac_sampling_rate, eSLAVE, e32xFS, eMCLK11_025KBASE, ePLLEnable);
            break;
    }
    if (codec_status != AUCODEC_STATUS_OK) {
        LOGE("aucodec_set_dai_sysclk failed\r\n");
    }

    //aucodec_set_output(eSpkOut);//Input: DACIN, Output:  speaker out
    aucodec_set_output(eLineOut);//Input: DACIN, Output:  aux out

    //must deinit i2c after configuring codec
    aucodec_i2c_deinit();

    if (codec_status == AUCODEC_STATUS_OK) {
        return AAC_DECODER_API_STATUS_OK;
    } else {
        return AAC_DECODER_API_STATUS_ERROR;
    }

}

static aac_decoder_api_status_t nau8810_close(void)
{
    LOGI("[CTRL]NAU8810 CLOSE \r\n");

    /*configure NAU8810*/
    AUCODEC_STATUS_e codec_status;
    hal_i2c_port_t i2c_port;
    hal_i2c_frequency_t frequency;

    codec_status = AUCODEC_STATUS_OK;
    i2c_port = HAL_I2C_MASTER_1;
    frequency = HAL_I2C_FREQUENCY_50K;

    codec_status = aucodec_i2c_init(i2c_port, frequency);
    if (codec_status != AUCODEC_STATUS_OK) {
        LOGE("aucodec_i2c_init failed\r\n");
    }

    aucodec_softreset();//soft reset

    //must deinit i2c after configuring codec
    aucodec_i2c_deinit();

    if (codec_status == AUCODEC_STATUS_OK) {
        return AAC_DECODER_API_STATUS_OK;
    } else {
        return AAC_DECODER_API_STATUS_ERROR;
    }

}

static aac_decoder_api_status_t i2s_configure(hal_i2s_channel_number_t hal_i2s_channel_number, hal_i2s_sample_rate_t hal_i2s_sample_rate)
{
    hal_i2s_config_t i2s_config;
    hal_i2s_status_t result = HAL_I2S_STATUS_OK;
    aac_decoder_api_status_t api_status;

    /* Set I2S as internal loopback mode */
    result = hal_i2s_init_ex(i2s_port, HAL_I2S_TYPE_EXTERNAL_MODE);
    if (HAL_I2S_STATUS_OK != result) {
        LOGE("hal_i2s_init_ex failed\r\n");
        return AAC_DECODER_API_STATUS_ERROR;
    }

    /* Configure I2S  */
    i2s_config.clock_mode = HAL_I2S_MASTER;
    i2s_config.sample_width = HAL_I2S_SAMPLE_WIDTH_16BIT;
    i2s_config.frame_sync_width = HAL_I2S_FRAME_SYNC_WIDTH_32;
    i2s_config.rx_down_rate = HAL_I2S_RX_DOWN_RATE_DISABLE;
    i2s_config.tx_mode = HAL_I2S_TX_MONO_DUPLICATE_DISABLE;
    i2s_config.i2s_out.channel_number = hal_i2s_channel_number;
    i2s_config.i2s_in.channel_number = hal_i2s_channel_number;

    i2s_config.i2s_out.sample_rate = hal_i2s_sample_rate;
    i2s_config.i2s_in.sample_rate = hal_i2s_sample_rate;
    i2s_config.i2s_in.msb_offset = 0;
    i2s_config.i2s_out.msb_offset = 0;
    i2s_config.i2s_in.word_select_inverse = HAL_I2S_WORD_SELECT_INVERSE_DISABLE;
    i2s_config.i2s_out.word_select_inverse = HAL_I2S_WORD_SELECT_INVERSE_DISABLE;
    i2s_config.i2s_in.lr_swap = HAL_I2S_LR_SWAP_DISABLE;
    i2s_config.i2s_out.lr_swap = HAL_I2S_LR_SWAP_DISABLE;

    result = hal_i2s_set_config_ex(i2s_port, &i2s_config);
    if (HAL_I2S_STATUS_OK != result) {
        LOGE("hal_i2s_set_config_ex failed\r\n");
        return AAC_DECODER_API_STATUS_ERROR;
    }

    api_status = nau8810_configure(translate_8810_sampling_rate(hal_i2s_sample_rate));
    if (api_status != AAC_DECODER_API_STATUS_OK) {
        LOGE("nau8810_configure ERROR\n");
        return AAC_DECODER_API_STATUS_ERROR;
    }

    return AAC_DECODER_API_STATUS_OK;
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
    LOGI("[CTRL]Deregister HISR callback \n");
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
        return ;
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

    api_status = i2s_configure(HAL_I2S_STEREO, translate_sampling_rate(internal_handle->info.sampling_rate));
    if (api_status != AAC_DECODER_API_STATUS_OK) {
        LOGE("i2s_configure ERROR\n");
        return AAC_DECODER_API_STATUS_ERROR;
    }

    i2s_tx_buffer = pvPortMallocNC(sizeof(uint32_t) * I2S_TX_BUFFER_LENGTH);
    if (i2s_tx_buffer == NULL) {
        LOGE("Alloc i2s tx buffer(%d) failed!!! \n", I2S_TX_BUFFER_LENGTH);
        return AAC_DECODER_API_STATUS_ERROR;
    }

    hal_i2s_setup_tx_vfifo_ex(i2s_port, i2s_tx_buffer, I2S_TX_BUFFER_LENGTH / 2, I2S_TX_BUFFER_LENGTH);
    i2s_tx_enable();
    aac_decoder_api_event_send_from_isr(AAC_DECODER_API_QUEUE_EVENT_DECODE, NULL);
    handle->state = AAC_DECODER_API_STATE_PLAY;
    return AAC_DECODER_API_STATUS_OK;

}


static aac_decoder_api_status_t aac_decoder_api_stop(aac_decoder_api_media_handle_t *handle)
{
    LOGI("[CTRL]STOP\r\n");

    aac_decoder_api_status_t api_status = AAC_DECODER_API_STATUS_OK;

    if (handle->state != AAC_DECODER_API_STATE_PLAY && handle->state != AAC_DECODER_API_STATE_PAUSE
            && handle->state != AAC_DECODER_API_STATE_RESUME) {
        LOGI("DECODER state error (%d)\r\n", handle->state);
        return AAC_DECODER_API_STATUS_ERROR;
    }
    i2s_tx_disable();
    api_status = nau8810_close();
    if (api_status != AAC_DECODER_API_STATUS_OK) {
        LOGE("nau8810_close ERROR\n");
        return AAC_DECODER_API_STATUS_ERROR;
    }
    aac_decoder_api_event_deregister_callback(AAC_DECODER_API_QUEUE_EVENT_DECODE);
    aac_decoder_api_reset_share_buffer(handle);
    aac_decoder_api_reset_pcm_out_buffer();

    if (i2s_tx_buffer != NULL) {
        vPortFreeNC(i2s_tx_buffer);
        i2s_tx_buffer = NULL;
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

    hal_i2s_disable_tx_dma_interrupt_ex(i2s_port);
    hal_i2s_disable_tx_ex(i2s_port);
    hal_i2s_disable_audio_top_ex(i2s_port);

    handle->state = AAC_DECODER_API_STATE_PAUSE;
    return AAC_DECODER_API_STATUS_OK;
}

static aac_decoder_api_status_t aac_decoder_api_resume(aac_decoder_api_media_handle_t *handle)
{
    LOGI("[CTRL]RESUME\n");

    if (handle->state != AAC_DECODER_API_STATE_PAUSE) {
        return AAC_DECODER_API_STATUS_ERROR;
    }

    hal_i2s_enable_tx_dma_interrupt_ex(i2s_port);
    hal_i2s_enable_tx_ex(i2s_port);
    hal_i2s_enable_audio_top_ex(i2s_port);

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


