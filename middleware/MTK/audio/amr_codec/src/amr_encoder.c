/* Copyright Statement:
 *
 * (C) 2005-2016 MediaTek Inc. All rights reserved.
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
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
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

/*Demo how to use amr codec to encode pcm stream to file, this part will likely be
  *   different in your application
 */
#include "amr_encoder.h"
#ifdef MTK_AUDIO_AMR_ENABLED
#include "amr_encoder_internal.h"
#include "hal_audio_internal_service.h"
#include "hal_gpt.h"

#define PCM_FRAME_SIZE                320
#define AMR_ENCODED_STREAM_OUT_SIZE   320 /*MAX_AMR_FRAME_SIZE*10*/

/*Must have it*/
//IPCOMMON_PLUS


/*For keeping global settings for encoder*/
static amr_encoder_internal_handle_t *amr_encoder_internal_handle = NULL;
static QueueHandle_t amr_encoder_queue_handle = NULL;
static uint8_t amr_encoder_queue_reg_num = 0;
static amr_encoder_queue_event_id_t amr_encoder_queue_event_id_array[MAX_AMR_ENCODER_FUNCTIONS];
static amr_encoder_internal_callback_t amr_encoder_queue_handler[MAX_AMR_ENCODER_FUNCTIONS];
static TaskHandle_t amr_encoder_task_handle = NULL;


static hal_gpt_port_t gpt_port = HAL_GPT_1;

static void amr_encoder_event_send_from_isr(amr_encoder_queue_event_id_t id, void *parameter);
static amr_encoder_status_t get_encoded_data_info(uint8_t **base_pointer, uint32_t *length);


void gpt_sw_callback(uint32_t *user_data)
{
    //LOGI("[CTRL]gpt_sw_callback \r\n");
    amr_encoder_internal_handle_t *internal_handle = amr_encoder_internal_handle;

    /*trigger encoder hisr*/
    uint32_t free_space_in_bytes = ring_buffer_get_space_byte_count(&internal_handle->encoded_pcm_buff);
    if (free_space_in_bytes >= 32) {
        amr_encoder_event_send_from_isr(AMR_ENCODER_QUEUE_EVENT_ENCODE, NULL);
    }
}


static void start_gpt_timer()
{
    LOGI("[CTRL]start_gpt_timer \r\n");
    hal_gpt_status_t   ret_status;
    ret_status = hal_gpt_init(gpt_port);
    if (ret_status != HAL_GPT_STATUS_OK) {
        LOGE("ERROR : Deep Sleep GPT Start Fail");
    }
    hal_gpt_register_callback(gpt_port, (hal_gpt_callback_t)gpt_sw_callback, NULL);
    hal_gpt_start_timer_ms(gpt_port, 50, HAL_GPT_TIMER_TYPE_REPEAT);
}

static void stop_gpt_timer()
{
    LOGI("[CTRL]stop_gpt_timer \r\n");
    hal_gpt_stop_timer(gpt_port);

}


static amr_encoder_status_t get_encoded_data_done(uint32_t length)
{
    //LOGI("get_encoded_data_done\r\n");
    amr_encoder_internal_handle_t *internal_handle = amr_encoder_internal_handle;
    amr_encoder_handle_t *handle = &internal_handle->handle;
    ring_buffer_read_done(&internal_handle->encoded_pcm_buff, length);
    handle->read_waiting = false;
    return AMR_ENCODER_STATUS_OK;

}

static amr_encoder_status_t get_encoded_data_info(uint8_t **base_pointer, uint32_t *length)
{
    //LOGI("get_encoded_data_info\r\n");

    amr_encoder_internal_handle_t *internal_handle = amr_encoder_internal_handle;

    ring_buffer_get_read_information(&internal_handle->encoded_pcm_buff, base_pointer, length);

    return AMR_ENCODER_STATUS_OK;
}


/*	share buffer operation function */
static void amr_encoder_set_share_buffer(amr_encoder_handle_t *handle, uint8_t *buffer, uint32_t length)
{
    handle->share_buffer.buffer_base = buffer;
    length &= ~0x1; // make buffer size even
    handle->share_buffer.buffer_size = length;
    handle->share_buffer.write = 0;
    handle->share_buffer.read = 0;
    handle->write_waiting = false;
    handle->underflow = false;
}


static void amr_encoder_get_write_buffer(amr_encoder_handle_t *handle, uint8_t **buffer, uint32_t *length)
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


static void amr_encoder_get_read_buffer(amr_encoder_handle_t *handle, uint8_t **buffer, uint32_t *length)
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


static void amr_encoder_write_data_done(amr_encoder_handle_t *handle, uint32_t length)
{
    handle->share_buffer.write += length;
    if (handle->share_buffer.write == handle->share_buffer.buffer_size) {
        handle->share_buffer.write = 0;
    }
}


static void amr_encoder_finish_write_data(amr_encoder_handle_t *handle)
{
    handle->write_waiting = false;
    handle->underflow = false;
}


static void amr_encoder_reset_share_buffer(amr_encoder_handle_t *handle)
{
    //memset(handle->share_buffer.buffer_base, 0, handle->share_buffer.buffer_size);  // do this or it will have previous data.
    handle->share_buffer.write = 0;
    handle->share_buffer.read = 0;
    handle->write_waiting = false;
    handle->underflow = false;
}


static void amr_encoder_read_data_done(amr_encoder_handle_t *handle, uint32_t length)
{
    handle->share_buffer.read += length;
    if (handle->share_buffer.read == handle->share_buffer.buffer_size) {
        handle->share_buffer.read = 0;
    }
}


static int32_t amr_encoder_get_free_space(amr_encoder_handle_t *handle)
{
    int32_t count = 0;

    count = handle->share_buffer.read - handle->share_buffer.write - 2;
    if (count < 0) {
        count += handle->share_buffer.buffer_size;
    }
    return count;
}


static int32_t amr_encoder_get_data_count(amr_encoder_handle_t *handle)
{
    int32_t count = 0;

    count = handle->share_buffer.write - handle->share_buffer.read;
    if (count < 0) {
        count += handle->share_buffer.buffer_size;
    }
    return count;
}


static void amr_decoder_reset_encoded_pcm_buffer(void)
{
    amr_encoder_internal_handle_t *internal_handle = amr_encoder_internal_handle;
    internal_handle->encoded_pcm_buff.read_pointer = 0;
    internal_handle->encoded_pcm_buff.write_pointer = 0;
}


static void amr_encoder_buffer_function_init(amr_encoder_handle_t *handle)
{
    handle->set_share_buffer       = amr_encoder_set_share_buffer;
    handle->get_write_buffer       = amr_encoder_get_write_buffer;
    handle->get_read_buffer        = amr_encoder_get_read_buffer;
    handle->write_data_done        = amr_encoder_write_data_done;
    handle->finish_write_data      = amr_encoder_finish_write_data;
    handle->reset_share_buffer     = amr_encoder_reset_share_buffer;
    handle->read_data_done         = amr_encoder_read_data_done;
    handle->get_free_space         = amr_encoder_get_free_space;
    handle->get_data_count         = amr_encoder_get_data_count;
}


static void amr_encoder_event_send_from_isr(amr_encoder_queue_event_id_t id, void *parameter)
{
    //LOGI("[CTRL]SEND HISR event\r\n");
    amr_encoder_queue_event_t event;
    event.id        = id;
    event.parameter = parameter;
    if (xQueueSendFromISR(amr_encoder_queue_handle, &event, 0) != pdPASS) {
        LOGI("queue not pass , id=%d\r\n", id);
        return;
    }

    return;
}


static void amr_encoder_event_register_callback(amr_encoder_queue_event_id_t reg_id, amr_encoder_internal_callback_t callback)
{
    uint32_t id_idx;
    for (id_idx = 0; id_idx < MAX_AMR_ENCODER_FUNCTIONS; id_idx++) {
        if (amr_encoder_queue_event_id_array[id_idx] == AMR_ENCODER_QUEUE_EVENT_NONE) {
            amr_encoder_queue_event_id_array[id_idx] = reg_id;
            amr_encoder_queue_handler[id_idx] = callback;
            amr_encoder_queue_reg_num++;
            break;
        }
    }
    return;
}


static void amr_encoder_event_deregister_callback(amr_encoder_queue_event_id_t dereg_id)
{
    LOGI("[CTRL]deregister HISR callback \r\n");
    uint32_t id_idx;
    for (id_idx = 0; id_idx < MAX_AMR_ENCODER_FUNCTIONS; id_idx++) {
        if (amr_encoder_queue_event_id_array[id_idx] == dereg_id) {
            amr_encoder_queue_event_id_array[id_idx] = AMR_ENCODER_QUEUE_EVENT_NONE;
            amr_encoder_queue_reg_num--;
            break;
        }
    }
    return;
}


static void amr_encoder_task_main(void *arg)
{
    LOGI("amr_encoder_task_main\r\n");

    amr_encoder_queue_event_t event;
    amr_encoder_queue_handle = xQueueCreate(AMR_ENCODER_QUEUE_SIZE, sizeof(amr_encoder_queue_event_t));

    /* Initialize queue registration */
    uint8_t id_idx;
    for (id_idx = 0; id_idx < MAX_AMR_ENCODER_FUNCTIONS; id_idx++) {
        amr_encoder_queue_event_id_array[id_idx] = AMR_ENCODER_QUEUE_EVENT_NONE;
    }

    while (1) {
        if (xQueueReceive(amr_encoder_queue_handle, &event, portMAX_DELAY)) {
            amr_encoder_queue_event_id_t rece_id = event.id;
            //LOGI("rece_id=%d\n",rece_id);
            uint8_t id_idx;
            for (id_idx = 0; id_idx < MAX_AMR_ENCODER_FUNCTIONS; id_idx++) {
                if (amr_encoder_queue_event_id_array[id_idx] == rece_id) {
                    //LOGI("find queue event id\n");
                    amr_encoder_queue_handler[id_idx](event.parameter);
                    break;
                }
            }
        }
    }
}


void amr_encoder_task_create(void)
{
    if (amr_encoder_task_handle ==  NULL) {
        LOGI("create amr encoder task\r\n");
        xTaskCreate(amr_encoder_task_main, AMR_ENCODER_TASK_NAME, AMR_ENCODER_TASK_STACKSIZE / sizeof(StackType_t), NULL, AMR_ENCODER_TASK_PRIO, &amr_encoder_task_handle);
    }

}


amr_encoder_status_t amr_encoder_get_memory_size(uint32_t *memory_size)
{
    uint32_t enc_int_buf_size = 0;
    uint32_t enc_tmp_buf_size = 0;
    uint32_t enc_bs_buf_size = 0;
    uint32_t enc_pcm_buf_size = 0;

    if (amr_encoder_internal_handle == NULL) {
        LOGE("amr_encoder_get_memory_size error,  amr_encoder_internal_handle should not be NULL\r\n");
        return AMR_ENCODER_STATUS_ERROR;
    }

    AMREnc_GetBufferSize((unsigned int *)&enc_int_buf_size,
                         (unsigned int *)&enc_tmp_buf_size,
                         (unsigned int *)&enc_bs_buf_size,
                         (unsigned int *)&enc_pcm_buf_size);

    *memory_size = enc_int_buf_size +
                   enc_tmp_buf_size +
                   enc_bs_buf_size +
                   enc_pcm_buf_size;

    return AMR_ENCODER_STATUS_OK;
}


static void amr_encoder_hisr_handler(void *data)
{
    //LOGI("amr_encoder_hisr_handler\r\n");

    amr_encoder_internal_handle_t *internal_handle = amr_encoder_internal_handle;
    amr_encoder_handle_t *handle = &internal_handle->handle;
    /*Ring buffer use*/
    uint32_t  remain = 0;
    uint32_t  consume = 0; //indicate how many bytes specified by user to read or write
    uint8_t   *share_buff_read_ptr = NULL;
    uint8_t   *P_dst = NULL;
    uint8_t   *P_src = NULL;
    uint32_t  share_buff_read_len = 0;
    uint32_t  total_share_buff_read_len = 0;
    uint32_t  packed_size_in_byte = 0;
    
    /*send request of refill share buffer*/
    if (amr_encoder_get_data_count(handle) <= handle->share_buffer.buffer_size / 2) {
        if (!handle->write_waiting) {
            handle->write_waiting = true;
            handle->handler(handle, AMR_ENCODER_MEDIA_EVENT_REQUEST);
        }
    }

    //LOGI("Prepare to encode\r\n");
    handle->state = AMR_ENCODER_STATE_ENCODING;
    if ((ring_buffer_get_space_byte_count(&internal_handle->encoded_pcm_buff)) >= 32) { //32bytes is the possible maximum size currently
        total_share_buff_read_len = handle->get_data_count(handle);
        //LOGI("total_share_buff_read_len =%d bytes\r\n",total_share_buff_read_len);
        if (total_share_buff_read_len < 320) {
            LOGW("total_share_buff_read_len < 320 bytes\r\n");
            if(internal_handle->flush_data_flag==0)
                return;
        }

        remain = PCM_FRAME_SIZE;
        P_dst = (uint8_t *)internal_handle->enc_pcm_buffer;
        for (uint32_t loop_idx = 0; loop_idx < 2; loop_idx++) {
            if (remain == 0) {
                break;
            }
            handle->get_read_buffer(handle, &share_buff_read_ptr, &share_buff_read_len);
            if (share_buff_read_len > 0) {
                //LOGI(" loop_idx=%d share_buff_read_len=%d\r\n",loop_idx,share_buff_read_len);
                consume = MINIMUM(share_buff_read_len, remain);
                //LOGI(" consume=%d \n",consume);
                memcpy(P_dst, share_buff_read_ptr, consume);
                handle->read_data_done(handle, consume);
                remain -= consume;
                P_dst += consume;
            }
        }

        /*encode*/
        packed_size_in_byte = (uint32_t)AMR_Encode(internal_handle->amr_enc_handle,
                              internal_handle->enc_tmp_buffer,
                              internal_handle->enc_pcm_buffer,
                              internal_handle->enc_bs_buffer,
                              (AMR_BitRate)internal_handle->bit_rate);

        internal_handle->frame++;
        //LOGI(" internal_handle->frame=%d \n",internal_handle->frame);
        //LOGI(" packed_size_in_byte=%d \n",packed_size_in_byte);
        /* Write deocded pcm to stream_out_buffer */
        remain = packed_size_in_byte;
        uint32_t free_space_in_bytes = 0;
        P_src = (uint8_t *)internal_handle->enc_bs_buffer;
        for (uint32_t loop_idx = 0; loop_idx < 2; loop_idx++) {
            if (remain == 0) {
                break;
            }
            ring_buffer_get_write_information(&internal_handle->encoded_pcm_buff, &P_dst, &free_space_in_bytes);
            //LOGI("loop_idx=%d P_dst=%x P_src=%x free_space_in_bytes=%d\n", loop_idx, P_dst, P_src, free_space_in_bytes);
            if (free_space_in_bytes > 0 && packed_size_in_byte > 0) {
                consume = MINIMUM(free_space_in_bytes, remain);
                //LOGI(" consume=%d \n",consume);
                memcpy(P_dst, P_src, consume);
                ring_buffer_write_done(&internal_handle->encoded_pcm_buff, consume);
                P_src += consume;
                remain -= consume;
            }
        }

        if (remain != 0) {
            LOGE("stream_out_pcm_buff space not enough for 1 frame, remain=%d!\n\r", remain);
        }
        //LOGI(" internal_handle->frame=%d \n",internal_handle->frame);
        //LOGI(" handle->read_waiting=%d \n",handle->read_waiting);
        if (internal_handle->frame >= 1) {
            if (!handle->read_waiting) {
                handle->read_waiting = true;
                handle->handler(handle, AMR_ENCODER_MEDIA_EVENT_DATA_NOTIFICATION);
            }
            internal_handle->frame = 0;
        }

        if(internal_handle->flush_data_flag==1){
            total_share_buff_read_len = handle->get_data_count(handle);
            //LOGI("total_share_buff_read_len =%d bytes\r\n",total_share_buff_read_len);
            if(total_share_buff_read_len==0)
               handle->handler(handle, AMR_ENCODER_MEDIA_EVENT_FLUSH_DONE);
        }

    }else{
        return;
    }

    /*trigger encoder hisr*/
    uint32_t free_space_in_bytes = ring_buffer_get_space_byte_count(&internal_handle->encoded_pcm_buff);
    if (free_space_in_bytes >= 32) {
        amr_encoder_event_send_from_isr(AMR_ENCODER_QUEUE_EVENT_ENCODE, NULL);
    }

}

static amr_encoder_status_t amr_encoder_stop(amr_encoder_handle_t *handle)
{
    LOGI("[CTRL]amr_encoder_stop \r\n");

    if (handle->state != AMR_ENCODER_STATE_READY  &&
            handle->state != AMR_ENCODER_STATE_ENCODING) {
        LOGE("[CTRL]amr_encoder_stop failed, state=%d\r\n", handle->state);
        return AMR_ENCODER_STATUS_ERROR;
    }
    stop_gpt_timer();

    handle->reset_share_buffer(handle);
    amr_decoder_reset_encoded_pcm_buffer();
    amr_encoder_event_deregister_callback(AMR_ENCODER_QUEUE_EVENT_ENCODE);
    handle->state = AMR_ENCODER_STATE_STOP;
    return AMR_ENCODER_STATUS_OK;
}

static amr_encoder_status_t amr_encoder_flush(amr_encoder_handle_t *handle,int32_t flush_data_flag)
{
     LOGI("[CTRL]amr_encoder_flush \r\n");
     amr_encoder_internal_handle_t *internal_handle = amr_encoder_internal_handle;
     internal_handle->flush_data_flag=flush_data_flag;
     
     return AMR_ENCODER_STATUS_OK;
}

static amr_encoder_status_t amr_encoder_start(amr_encoder_handle_t *handle)
{
    LOGI("[CTRL]amr_encoder_start \r\n");
    if (handle->state != AMR_ENCODER_STATE_READY) {
        LOGE("codec state != AMR_ENCODER_STATE_READY\r\n");
        return AMR_ENCODER_STATUS_ERROR;
    }


    amr_encoder_internal_handle_t *internal_handle = (amr_encoder_internal_handle_t *) handle;
    amr_encoder_event_register_callback(AMR_ENCODER_QUEUE_EVENT_ENCODE, amr_encoder_hisr_handler);

    internal_handle->flush_data_flag=0;
    internal_handle->amr_enc_handle = AMREnc_Init(internal_handle->enc_int_buffer, (AMR_BitRate)internal_handle->bit_rate, internal_handle->dtx_enable);
    if (internal_handle->amr_enc_handle == NULL) {
        LOGE("amr_enc_handle == NULL\r\n");
        return AMR_ENCODER_STATUS_ERROR;
    }

    start_gpt_timer();
    uint32_t free_space_in_bytes = ring_buffer_get_space_byte_count(&internal_handle->encoded_pcm_buff);
    if (free_space_in_bytes >= 32) {
        amr_encoder_event_send_from_isr(AMR_ENCODER_QUEUE_EVENT_ENCODE, NULL);
    }
    return AMR_ENCODER_STATUS_OK;
}


amr_encoder_handle_t *amr_encoder_open(amr_encoder_callback_t amr_encoder_callback, const amr_encoder_config_t encoder_config)
{
    LOGI("[CTRL]amr_encoder_open \r\n");

    amr_encoder_handle_t *handle;
    amr_encoder_internal_handle_t *internal_handle;

    /* alloc internal handler space and initialize it*/
    internal_handle = (amr_encoder_internal_handle_t *)pvPortMalloc(sizeof(amr_encoder_internal_handle_t));
    if (internal_handle == NULL) {
        LOGE("pvPortMalloc internal_handle failed\r\n");
        return NULL;
    }
    memset(internal_handle, 0, sizeof(amr_encoder_internal_handle_t));

    /* assign internal handler to be global and static handler*/
    amr_encoder_internal_handle = internal_handle;

    /* initialize internal handle*/
    internal_handle->enc_int_buf_size = 0;
    internal_handle->enc_tmp_buf_size = 0;
    internal_handle->enc_pcm_buf_size = 0;
    internal_handle->enc_bs_buf_size = 0;
    internal_handle->encoded_pcm_buff_size = 0;
    internal_handle->frame = 0;

    /* initialize handle*/
    handle = &internal_handle->handle;
    handle->handler = amr_encoder_callback;
    handle->audio_id = 0x1002;


    /*decoder*/
    handle->encoder_flush = amr_encoder_flush;
    handle->encoder_start = amr_encoder_start;
    handle->encoder_stop  = amr_encoder_stop;
    handle->get_encoded_data_info = get_encoded_data_info;
    handle->get_encoded_data_done = get_encoded_data_done;

    /*initialize buffer control function*/
    amr_encoder_buffer_function_init(handle);

    /*create codec task*/
    amr_encoder_task_create();

    AMREnc_GetBufferSize((unsigned int *)&internal_handle->enc_int_buf_size,
                         (unsigned int *)&internal_handle->enc_tmp_buf_size,
                         (unsigned int *)&internal_handle->enc_bs_buf_size,
                         (unsigned int *)&internal_handle->enc_pcm_buf_size);

    /*allocate memory for stream out pcm*/
    internal_handle->encoded_pcm_buff_size = AMR_ENCODED_STREAM_OUT_SIZE;
    internal_handle->encoded_pcm_buff.buffer_byte_count = internal_handle->encoded_pcm_buff_size;
    internal_handle->encoded_pcm_buff.read_pointer = 0;
    internal_handle->encoded_pcm_buff.write_pointer = 0;
    internal_handle->encoded_pcm_buff.buffer_base_pointer  = (uint8_t *)pvPortMalloc(internal_handle->encoded_pcm_buff_size * sizeof(uint8_t));
    if (internal_handle->encoded_pcm_buff.buffer_base_pointer == NULL) {
        LOGE("pvPortMalloc encoded_pcm_buff failed\r\n");
        return NULL;
    }

    /*allocate memory for encoder*/
    internal_handle->enc_int_buffer = (uint8_t *)pvPortMalloc(internal_handle->enc_int_buf_size * sizeof(uint8_t));
    if (internal_handle->enc_int_buffer == NULL) {
        LOGE("pvPortMalloc enc_int_buffer failed\r\n");
        return NULL;
    }

    internal_handle->enc_tmp_buffer = (uint8_t *)pvPortMalloc(internal_handle->enc_tmp_buf_size * sizeof(uint8_t));
    if (internal_handle->enc_tmp_buffer == NULL) {
        LOGE("pvPortMalloc enc_tmp_buffer failed\r\n");
        return NULL;
    }

    internal_handle->enc_pcm_buffer = (int16_t *)pvPortMalloc(internal_handle->enc_pcm_buf_size * sizeof(uint8_t));
    if (internal_handle->enc_pcm_buffer == NULL) {
        LOGE("pvPortMalloc enc_pcm_buffer failed\r\n");
        return NULL;
    }

    internal_handle->enc_bs_buffer  = (uint8_t *)pvPortMalloc(internal_handle->enc_bs_buf_size * sizeof(uint8_t));
    if (internal_handle->enc_bs_buffer == NULL) {
        LOGE("pvPortMalloc enc_bs_buffer failed\r\n");
        return NULL;
    }

    LOGI("internal_handle->enc_int_buffer=%x size=%d\r\n", internal_handle->enc_int_buffer , internal_handle->enc_int_buf_size);
    LOGI("internal_handle->enc_tmp_buffer=%x size=%d\r\n", internal_handle->enc_tmp_buffer , internal_handle->enc_tmp_buf_size);
    LOGI("internal_handle->enc_pcm_buffer=%x size=%d\r\n", internal_handle->enc_pcm_buffer , internal_handle->enc_pcm_buf_size);
    LOGI("internal_handle->enc_bs_buffer=%x size=%d\r\n", internal_handle->enc_bs_buffer , internal_handle->enc_bs_buf_size);
    LOGI("internal_handle->encoded_pcm_buff.buffer_base_pointer=%x size=%d\r\n", internal_handle->encoded_pcm_buff.buffer_base_pointer, internal_handle->encoded_pcm_buff_size);

    internal_handle->bit_rate = (AMR_BitRate)encoder_config.bitrate;
    internal_handle->dtx_enable = (int32_t)encoder_config.dtx_mode;
    /*codec state*/
    handle->state = AMR_ENCODER_STATE_READY;

    return handle;
}


amr_encoder_status_t amr_encoder_close(amr_encoder_handle_t *handle)
{
    LOGI("[CTRL]amr_encoder_close \r\n");

    amr_encoder_internal_handle_t *internal_handle = (amr_encoder_internal_handle_t *) handle;

    if (handle->state != AMR_ENCODER_STATE_STOP && handle->state != AMR_ENCODER_STATE_READY) {
        return AMR_ENCODER_STATUS_ERROR;
    }

    handle->state = AMR_ENCODER_STATE_IDLE;

    if (amr_encoder_queue_handle != NULL) {
        vQueueDelete(amr_encoder_queue_handle);
        amr_encoder_queue_handle = NULL;
    }

    if (amr_encoder_task_handle != NULL) {
        vTaskDelete(amr_encoder_task_handle);
        amr_encoder_task_handle = NULL;
    }

    vPortFree(internal_handle);
    internal_handle = NULL;
    amr_encoder_internal_handle = NULL;

    return AMR_ENCODER_STATUS_OK;

}


#endif/*MTK_AUDIO_AMR_DECODER_ENABLED*/

