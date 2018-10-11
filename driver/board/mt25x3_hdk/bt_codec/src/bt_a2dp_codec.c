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

#include "bt_a2dp_codec_internal.h"
//#define BT_A2DP_BITSTREAM_DUMP_DEBUG
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
/* #define AWS_DEBUG_CODE */
//#define AWS_DEBUG_CODE
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */

#ifdef MTK_BT_A2DP_SOURCE_SUPPORT
#include "sbc_codec.h"
#endif /*MTK_BT_A2DP_SOURCE_SUPPORT*/

#ifdef MTK_BT_A2DP_SOURCE_SUPPORT
const bt_codec_sbc_t source_capability_sbc[1] = {
    {
        25,  /* min_bit_pool       */
        75,  /* max_bit_pool       */
        0x1, /* block_len: 16 only     */
        0x1, /* subband_num: 8 only   */
        SBC_ALLOCATION_SUPPORT,
        0xf, /* sample_rate: all   */
        0xf  /* channel_mode: all  */
    }
};
#endif /*MTK_BT_A2DP_SOURCE_SUPPORT*/

const bt_codec_sbc_t sink_capability_sbc[1] = {
    {
        18,  /* min_bit_pool       */
        75,  /* max_bit_pool       */
        0xf, /* block_len: all     */
        0xf, /* subband_num: all   */
        0x3, /* both snr/loudness  */
        0xf, /* sample_rate: all   */
        0xf  /* channel_mode: all  */
    }
};

const bt_codec_aac_t sink_capability_aac[1] = {
    {
        true,    /*VBR         */
        0xc0,    /*Object type */
        0x03,    /*Channels    */
        0x0ff8,  /*Sample_rate */
        0x60000  /*bit_rate, 384 Kbps */
    }
};

static void bt_set_buffer(bt_media_handle_t *handle, uint8_t *buffer, uint32_t length)
{
    handle->buffer_info.buffer_base = buffer;
    length &= ~0x1; // make buffer size even
    handle->buffer_info.buffer_size = length;
    handle->buffer_info.write = 0;
    handle->buffer_info.read = 0;
    handle->waiting = false;
    handle->underflow = false;
}

static void bt_set_get_data_function(bt_media_handle_t *handle, bt_codec_get_data func)
{
    handle->directly_access_dsp_function = func;
}

static void bt_set_get_data_count_function(bt_media_handle_t *handle, bt_codec_get_data_count func)
{
    handle->get_data_count_function = func;
}

static void bt_get_write_buffer(bt_media_handle_t *handle, uint8_t **buffer, uint32_t *length)
{
    int32_t count = 0;

    if (handle->buffer_info.read > handle->buffer_info.write) {
        count = handle->buffer_info.read - handle->buffer_info.write - 1;
    } else if (handle->buffer_info.read == 0) {
        count = handle->buffer_info.buffer_size - handle->buffer_info.write - 1;
    } else {
        count = handle->buffer_info.buffer_size - handle->buffer_info.write;
    }
    *buffer = handle->buffer_info.buffer_base + handle->buffer_info.write;
    *length = count;
}

static void bt_get_read_buffer(bt_media_handle_t *handle, uint8_t **buffer, uint32_t *length)
{
    int32_t count = 0;

    if (handle->buffer_info.write >= handle->buffer_info.read) {
        count = handle->buffer_info.write - handle->buffer_info.read;
    } else {
        count = handle->buffer_info.buffer_size - handle->buffer_info.read;
    }
    *buffer = handle->buffer_info.buffer_base + handle->buffer_info.read;
    *length = count;
}

static void bt_write_data_done(bt_media_handle_t *handle, uint32_t length)
{
    handle->buffer_info.write += length;
#ifdef BT_A2DP_BITSTREAM_DUMP_DEBUG
    TASK_LOG_I("[A2DP]write--wr: %d, len: %d\n", handle->buffer_info.write, length);
#endif
    if (handle->buffer_info.write == handle->buffer_info.buffer_size) {
        handle->buffer_info.write = 0;
    }
}

static void bt_finish_write_data(bt_media_handle_t *handle)
{
    handle->waiting = false;
    handle->underflow = false;
}

static void bt_reset_share_buffer(bt_media_handle_t *handle)
{
    handle->buffer_info.write = 0;
    handle->buffer_info.read = 0;
    handle->waiting = false;
    handle->underflow = false;
}

static void bt_read_data_done(bt_media_handle_t *handle, uint32_t length)
{
    handle->buffer_info.read += length;
#ifdef BT_A2DP_BITSTREAM_DUMP_DEBUG
    TASK_LOG_I("[A2DP]read--rd: %d, len: %d\n", handle->buffer_info.read, length);
#endif
    if (handle->buffer_info.read == handle->buffer_info.buffer_size) {
        handle->buffer_info.read = 0;
    }
}

static int32_t bt_get_free_space(bt_media_handle_t *handle)
{
    int32_t count = 0;

    count = handle->buffer_info.read - handle->buffer_info.write - 1;
    if (count < 0) {
        count += handle->buffer_info.buffer_size;
    }
    return count;
}

static int32_t bt_get_data_count(bt_media_handle_t *handle)
{
    int32_t count = 0;

    count = handle->buffer_info.write - handle->buffer_info.read;
    if (count < 0) {
        count += handle->buffer_info.buffer_size;
    }
    return count;
}

static void bt_codec_buffer_function_init(bt_media_handle_t *handle)
{
    handle->set_buffer         = bt_set_buffer;
    handle->set_get_data_function = bt_set_get_data_function;
    handle->set_get_data_count_function = bt_set_get_data_count_function;
    handle->get_write_buffer   = bt_get_write_buffer;
    handle->get_read_buffer    = bt_get_read_buffer;
    handle->write_data_done    = bt_write_data_done;
    handle->finish_write_data  = bt_finish_write_data;
    handle->reset_share_buffer = bt_reset_share_buffer;
    handle->read_data_done     = bt_read_data_done;
    handle->get_free_space     = bt_get_free_space;
    handle->get_data_count     = bt_get_data_count;
}

#if defined(BT_A2DP_BITSTREAM_DUMP_DEBUG)
#define BT_A2DP_BS_LEN 160000
uint32_t bt_a2dp_ptr = 0;
uint8_t bt_a2dp_bitstream[BT_A2DP_BS_LEN];
#endif /* BT_A2DP_BITSTREAM_DUMP_DEBUG */

#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
static uint32_t bt_codec_a2dp_aws_convert_sampling_rate_from_index_to_value(uint32_t sampling_rate_index)
{
    uint32_t sampling_rate_value;
    if (sampling_rate_index == HAL_AUDIO_SAMPLING_RATE_8KHZ) {
        sampling_rate_value = 8000;
    } else if (sampling_rate_index == HAL_AUDIO_SAMPLING_RATE_11_025KHZ) {
        sampling_rate_value = 11025;
    } else if (sampling_rate_index == HAL_AUDIO_SAMPLING_RATE_12KHZ) {
        sampling_rate_value = 12000;
    } else if (sampling_rate_index == HAL_AUDIO_SAMPLING_RATE_16KHZ) {
        sampling_rate_value = 16000;
    } else if (sampling_rate_index == HAL_AUDIO_SAMPLING_RATE_22_05KHZ) {
        sampling_rate_value = 22050;
    } else if (sampling_rate_index == HAL_AUDIO_SAMPLING_RATE_24KHZ) {
        sampling_rate_value = 24000;
    } else if (sampling_rate_index == HAL_AUDIO_SAMPLING_RATE_32KHZ) {
        sampling_rate_value = 32000;
    } else if (sampling_rate_index == HAL_AUDIO_SAMPLING_RATE_44_1KHZ) {
        sampling_rate_value = 44100;
    } else {
        sampling_rate_value = 48000;
    }
    return sampling_rate_value;
}



static void bt_codec_a2dp_aws_callback(aws_event_t event, void *user_data)
{
    bt_a2dp_audio_internal_handle_t *p_info = (bt_a2dp_audio_internal_handle_t *)user_data;
    bt_media_handle_t *handle = (bt_media_handle_t *)p_info;
    switch (event) {
        case CODEC_AWS_CHECK_CLOCK_SKEW:
            handle->handler(handle, BT_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW);
            break;
        case CODEC_AWS_CHECK_UNDERFLOW:
            handle->handler(handle, BT_CODEC_MEDIA_AWS_CHECK_UNDERFLOW);
            break;
        default:
            break;
    }
    return;
}

static int32_t bt_codec_a2dp_aws_open_setting(bt_a2dp_audio_internal_handle_t *p_info)
{
    int32_t result = 0;
    bt_codec_a2dp_audio_t *p_codec = &p_info->codec_info;
    uint8_t *p_aws_buf = NULL;
    int32_t aws_buf_size = 0;
    p_info->aws_flag = false;
    p_info->aws_internal_flag = false;
    p_info->aws_init_sync_flag = false;
    if (p_codec->role == BT_A2DP_SINK) {
        bt_codec_capability_t *p_cap = &p_codec->codec_cap;
        bt_a2dp_codec_type_t type = p_cap->type;
        if (type == BT_A2DP_CODEC_SBC) {
            aws_buf_size = audio_service_aws_get_buffer_size(AWS_CODEC_TYPE_SBC_FORMAT);
            if (aws_buf_size < 0) {
                TASK_LOG_E("[AWS] Fail: Buffer size is negative\n");
                return -1;
            }
            p_info->aws_working_buf_size = aws_buf_size;
            p_aws_buf = (uint8_t *)pvPortMalloc(aws_buf_size);
            if (p_aws_buf == NULL) {
                TASK_LOG_E("[AWS] Fail: Aws buf allocation failed\n");
                return -2;
            }
            p_info->aws_working_buffer = p_aws_buf;
            audio_service_aws_init(p_aws_buf, AWS_CODEC_TYPE_SBC_FORMAT, (aws_callback_t)bt_codec_a2dp_aws_callback, (void *)p_info);
        }
#if defined(MTK_BT_A2DP_AAC_ENABLE)
        else if (type == BT_A2DP_CODEC_AAC) {
            aws_buf_size = audio_service_aws_get_buffer_size(AWS_CODEC_TYPE_AAC_FORMAT);
            if (aws_buf_size < 0) {
                TASK_LOG_E("[AWS] Fail: Buffer size is negative\n");
                return -1;
            }
            p_info->aws_working_buf_size = aws_buf_size;
            p_aws_buf = (uint8_t *)pvPortMalloc(aws_buf_size);
            if (p_aws_buf == NULL) {
                TASK_LOG_E("[AWS] Fail: Aws buf allocation failed\n");
                return -2;
            }
            p_info->aws_working_buffer = p_aws_buf;
            audio_service_aws_init(p_aws_buf, AWS_CODEC_TYPE_AAC_FORMAT, (aws_callback_t)bt_codec_a2dp_aws_callback, (void *)p_info);
        }
#endif  /* defined(MTK_BT_A2DP_AAC_ENABLE) */
        else {  /* Invalid codec type */
            result = -1;
        }
    }
    return result;
}

static void bt_codec_a2dp_aws_close_setting(bt_a2dp_audio_internal_handle_t *p_info)
{
    uint8_t *p_aws_buf = p_info->aws_working_buffer;
    audio_service_aws_deinit();
    if (p_aws_buf != NULL) {
        vPortFree(p_aws_buf);
        p_info->aws_working_buffer = (uint8_t *)NULL;
    }
    return;
}

static void bt_codec_a2dp_aws_play_setting(bt_a2dp_audio_internal_handle_t *p_info)
{
    p_info->aws_internal_flag = true;
    audio_service_aws_set_flag(true);
    return;
}

static void bt_codec_a2dp_aws_stop_setting(bt_a2dp_audio_internal_handle_t *p_info)
{
    audio_service_aws_set_flag(false);
    p_info->aws_internal_flag = false;
    p_info->aws_init_sync_flag = false;
    return;
}
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */

static void bt_write_bs_to_dsp(bt_a2dp_audio_internal_handle_t *internal_handle)
{
    bt_media_handle_t *handle = (bt_media_handle_t *)internal_handle;
    bt_a2dp_codec_type_t codec_type = internal_handle->codec_info.codec_cap.type;
    ring_buffer_information_t *p_ring = &internal_handle->ring_info;
    uint16_t bs_page;
    uint16_t bs_addr;
    uint16_t bs_size;
    uint16_t bs_wptr;
    uint16_t bs_rptr;
    if (codec_type == BT_A2DP_CODEC_SBC) {
        bs_page = *DSP_SBC_DEC_DM_BS_PAGE;
        bs_addr = *DSP_SBC_DEC_DM_BS_ADDR;
        bs_size = *DSP_SBC_DEC_DM_BS_LEN;
        bs_wptr = *DSP_SBC_DEC_DM_BS_MCU_W_PTR;
        bs_rptr = *DSP_SBC_DEC_DM_BS_DSP_R_PTR;
        p_ring->write_pointer       = (uint32_t)bs_wptr;
        p_ring->read_pointer        = (uint32_t)bs_rptr;
        p_ring->buffer_byte_count   = (uint32_t)bs_size;
        p_ring->buffer_base_pointer = (uint8_t *)DSP_DM_ADDR(bs_page, bs_addr);
    }
#ifdef MTK_BT_A2DP_AAC_ENABLE
    else if (codec_type == BT_A2DP_CODEC_AAC) {
        bs_addr = *DSP_AAC_DEC_DM_BS_ADDR;
        bs_wptr = *DSP_AAC_DEC_DM_BS_MCU_W_PTR;    // in word
        bs_rptr = *DSP_AAC_DEC_DM_BS_DSP_R_PTR;    // in word
        p_ring->write_pointer       = (uint32_t)((bs_wptr - bs_addr) << 1); // in byte
        p_ring->read_pointer        = (uint32_t)((bs_rptr - bs_addr) << 1);
    }
#endif /*MTK_BT_A2DP_AAC_ENABLE*/
    {
        uint32_t loop_idx;
        uint32_t loop_cnt = 4;
        for (loop_idx = 0; loop_idx < loop_cnt; loop_idx++) {
            uint32_t           read_byte_cnt  = 0;
            uint32_t           write_byte_cnt = 0;
            uint32_t           move_byte_cnt = 0;
            uint8_t           *p_mcu_buf      = NULL;
            volatile uint16_t *p_dsp_buf      = NULL;

            if (codec_type == BT_A2DP_CODEC_SBC) {
                ring_buffer_get_write_information(p_ring, (uint8_t **)&p_dsp_buf, &write_byte_cnt);
            }
#ifdef MTK_BT_A2DP_AAC_ENABLE
            else if (codec_type == BT_A2DP_CODEC_AAC) {
                ring_buffer_get_write_information_non_mirroring(p_ring, (uint8_t **)&p_dsp_buf, &write_byte_cnt);
            }
#endif /*MTK_BT_A2DP_AAC_ENABLE*/

            write_byte_cnt &= ~0x1;     // Make it even

            if (handle->directly_access_dsp_function == NULL) {
                handle->get_read_buffer(handle, &p_mcu_buf, &read_byte_cnt);
                read_byte_cnt  &= ~0x1;     // Make it even

                move_byte_cnt = MINIMUM(write_byte_cnt, read_byte_cnt);
                {
                    // Move data
                    uint32_t move_word_cnt = move_byte_cnt >> 1;
                    if (move_word_cnt > 0) {
#if defined(BT_A2DP_BITSTREAM_DUMP_DEBUG)
                        if (bt_a2dp_ptr + move_byte_cnt <= BT_A2DP_BS_LEN) {
                            memcpy(bt_a2dp_bitstream + bt_a2dp_ptr, p_mcu_buf, move_byte_cnt);
                            bt_a2dp_ptr += move_byte_cnt;
                        }
#endif /* BT_A2DP_BITSTREAM_DUMP_DEBUG */
                        audio_idma_write_to_dsp(p_dsp_buf, (uint16_t *)p_mcu_buf, move_word_cnt);
                    } else {    // Read buffer empty or write buffer full
                        break;
                    }
                }

                handle->read_data_done(handle, move_byte_cnt);
            } else {
                uint32_t write_word_cnt = write_byte_cnt >> 1;
                uint32_t wrote_word_cnt = 0;    // how many word written by directly_access_dsp_function
                wrote_word_cnt = handle->directly_access_dsp_function(p_dsp_buf, write_word_cnt);
                move_byte_cnt = wrote_word_cnt << 1;
                if (wrote_word_cnt == 0) {
                    break;
                }
            }

            if (codec_type == BT_A2DP_CODEC_SBC) {
                ring_buffer_write_done(p_ring, move_byte_cnt);
                *DSP_SBC_DEC_DM_BS_MCU_W_PTR = (uint16_t)p_ring->write_pointer;
            }
#ifdef MTK_BT_A2DP_AAC_ENABLE
            else if (codec_type == BT_A2DP_CODEC_AAC) {
                ring_buffer_write_done_non_mirroring(p_ring, move_byte_cnt);
                *DSP_AAC_DEC_DM_BS_MCU_W_PTR = (uint16_t)(p_ring->write_pointer >> 1) + bs_addr;
            }
#endif /*MTK_BT_A2DP_AAC_ENABLE*/
        }
    }
    *DSP_TASK4_COSIM_HANDSHAKE = 0;

    return;
}

static void sbc_decoder_isr_handler(void *data)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)data;
    bt_media_handle_t *handle = (bt_media_handle_t *)internal_handle;
#ifdef BT_A2DP_BITSTREAM_DUMP_DEBUG
    TASK_LOG_I("[A2DP][SBC]ISR");
#endif
    if ((GET_DSP_VALUE(RG_SBC_DEC_FSM) == DSP_SBC_STATE_IDLE) || (handle->state == BT_CODEC_STATE_ERROR)) {
        return;
    }

    /* error handling */
    if (GET_DSP_VALUE(RG_SBC_DEC_FSM) == DSP_SBC_STATE_RUNNING) {
        if ((GET_DSP_VALUE(RG_SBC_DEC_STATUS) == DSP_SBC_DEC_SYNC_ERR)
                || (GET_DSP_VALUE(RG_SBC_DEC_STATUS) == DSP_SBC_DEC_CRC_ERR)
                || (GET_DSP_VALUE(RG_SBC_DEC_STATUS) == DSP_SBC_DEC_BITPOOL_ERR)
                || (GET_DSP_VALUE(RG_SBC_PAR_STATUS) == DSP_SBC_PAR_MAGIC_WORD_ERR)
                || (GET_DSP_VALUE(RG_SBC_PAR_STATUS) == DSP_SBC_PAR_INVALID_NON_FRAGMENTED_PAYLOAD)
                || (GET_DSP_VALUE(RG_SBC_PAR_STATUS) == DSP_SBC_PAR_START_PAYLOAD_ERR)
                || (GET_DSP_VALUE(RG_SBC_PAR_STATUS) == DSP_SBC_PAR_INVALID_FRAGMENT_INDEX_1)
                || (GET_DSP_VALUE(RG_SBC_PAR_STATUS) == DSP_SBC_PAR_INVALID_FRAGMENT_INDEX_2)
                || (GET_DSP_VALUE(RG_SBC_PAR_STATUS) == DSP_SBC_PAR_INVALID_FRAME_NUMBER)
           ) {
            LISR_LOG_E("[A2DP][SBC]DECODER ERR, PAR:%d  DEC=%d\n", GET_DSP_VALUE(RG_SBC_PAR_STATUS), GET_DSP_VALUE(RG_SBC_DEC_STATUS));
            handle->handler(handle, BT_CODEC_MEDIA_ERROR);
            handle->state = BT_CODEC_STATE_ERROR;
            return;
        }
    }
    internal_handle->frame_count++;
    /* fill bitstream */
    bt_write_bs_to_dsp(internal_handle);
    if (handle->directly_access_dsp_function == NULL) {//Share buffer flow
        if (!handle->waiting) {
            handle->waiting = true;
            if ((!handle->underflow) && (GET_DSP_VALUE(RG_SBC_DEC_STATUS) == DSP_SBC_DEC_BS_UNDERFLOW)) {
                handle->underflow = true;
                LISR_LOG_I("[A2DP][SBC] DSP underflow \n");
                handle->handler(handle, BT_CODEC_MEDIA_UNDERFLOW);
            } else {
                handle->handler(handle, BT_CODEC_MEDIA_REQUEST);
            }
        }
    }
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    else { //Direct access DSP flow
        if (GET_DSP_VALUE(RG_SBC_DEC_STATUS) == DSP_SBC_DEC_BS_UNDERFLOW) {
            if (internal_handle->aws_internal_flag) {
                LISR_LOG_I("[A2DP][SBC] DSP underflow \n");
                handle->handler(handle, BT_CODEC_MEDIA_UNDERFLOW);
            }
        }
    }
#endif
}

static bt_codec_media_status_t bt_open_sink_sbc_codec(bt_media_handle_t *handle)
{
    uint16_t I = 0;
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;

    TASK_LOG_CTRL("[SBC]open_codec\r\n");
    audio_service_hook_isr(DSP_D2C_SBC_DEC_INT, sbc_decoder_isr_handler, internal_handle);
    audio_service_setflag(handle->audio_id);
    if (GET_DSP_VALUE(RG_SBC_DEC_FSM) != DSP_SBC_STATE_IDLE) {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
#if defined(AWS_DEBUG_CODE)
    TASK_LOG_I("[AWS] set AWS flag\r\n");
    bt_codec_a2dp_aws_set_flag(handle, true);
#endif  /* defined(AWS_DEBUG_CODE) */
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    if (internal_handle->aws_flag) {
        bt_codec_a2dp_aws_play_setting(internal_handle);
    }
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    internal_handle->frame_count = 0;
    *DSP_AUDIO_FLEXI_CTRL |= (FLEXI_VBI_ENABLE | FLEXI_SD_ENABLE);

    *DSP_SBC_PAR_MAGIC_WORD = 0x3453;
    SET_DSP_VALUE(RG_SBC_PARSER_EN, 1);
    SET_DSP_VALUE(RG_SBC_DEC_FSM, DSP_SBC_STATE_START);
    *DSP_SBC_DEC_DM_BS_MCU_W_PTR = 0;
    dsp_audio_fw_dynamic_download(DDID_SBC_DECODE);
    afe_set_path_type(HAL_AUDIO_PLAYBACK_MUSIC);
    audio_playback_on(ASP_TYPE_SBC_DEC, internal_handle->sample_rate);
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    if (internal_handle->aws_internal_flag == false)
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    {
        for (I = 0; ; I++) {
            if (GET_DSP_VALUE(RG_SBC_DEC_FSM) == DSP_SBC_STATE_RUNNING) {
                break;
            }
            if (I > 80) {
                return BT_CODEC_MEDIA_STATUS_ERROR;
            }
            hal_gpt_delay_ms(9);
        }
    }
#if defined(AWS_DEBUG_CODE)
    else {  /* Manually reset DSP for unit test */
        bt_codec_a2dp_aws_set_initial_sync(handle);
        for (I = 0; ; I++) {
            if (GET_DSP_VALUE(RG_SBC_DEC_FSM) == DSP_SBC_STATE_RUNNING) {
                break;
            }
            if (I > 80) {
                return BT_CODEC_MEDIA_STATUS_ERROR;
            }
            hal_gpt_delay_ms(9);
        }
    }
#endif  /* defined(AWS_DEBUG_CODE) */
    handle->state = BT_CODEC_STATE_PLAY;
    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_close_sink_sbc_codec(bt_media_handle_t *handle)
{
    uint16_t I = 0;
    TASK_LOG_CTRL("[SBC]close_codec\r\n");
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    {
        bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)handle;
        if (internal_handle->aws_internal_flag && !internal_handle->aws_flag && !internal_handle->aws_init_sync_flag) {
            SET_DSP_VALUE(RG_SBC_DEC_FSM, DSP_SBC_STATE_IDLE);
        }
    }
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    for (I = 0; ; I++) {
        if (GET_DSP_VALUE(RG_SBC_DEC_FSM) == DSP_SBC_STATE_IDLE) {
            break;
        }
        if (GET_DSP_VALUE(RG_SBC_DEC_FSM) == DSP_SBC_STATE_RUNNING) {
            SET_DSP_VALUE(RG_SBC_DEC_FSM, DSP_SBC_STATE_FLUSH);
        }
        if (I > 80) {
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
        hal_gpt_delay_ms(9);
    }
    SET_DSP_VALUE(RG_SBC_PARSER_EN, 0);

    *DSP_AUDIO_CTRL2 &= ~DSP_PCM_R_DIS;
    *DSP_AUDIO_FLEXI_CTRL &= ~(FLEXI_VBI_ENABLE | FLEXI_SD_ENABLE);

    audio_playback_off();
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    {
        bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)handle;
        if (internal_handle->aws_internal_flag) {
            bt_codec_a2dp_aws_stop_setting(internal_handle);
        }
    }
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
#if defined(AWS_DEBUG_CODE)
    TASK_LOG_I("[AWS] clear AWS flag\r\n");
    bt_codec_a2dp_aws_set_flag(handle, false);
#endif  /* defined(AWS_DEBUG_CODE) */
    audio_service_unhook_isr(DSP_D2C_SBC_DEC_INT);
    audio_service_clearflag(handle->audio_id);
    handle->state = BT_CODEC_STATE_STOP;
    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_a2dp_sink_sbc_play(bt_media_handle_t *handle)
{
    TASK_LOG_CTRL("[SBC]play\r\n");
    if (handle->state != BT_CODEC_STATE_READY && handle->state != BT_CODEC_STATE_STOP) {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
#if defined(BT_A2DP_BITSTREAM_DUMP_DEBUG)
    bt_a2dp_ptr = 0;
    memset(bt_a2dp_bitstream, 0, BT_A2DP_BS_LEN * sizeof(uint8_t));
#endif /* BT_A2DP_BITSTREAM_DUMP_DEBUG */
    return bt_open_sink_sbc_codec(handle);
}

static bt_codec_media_status_t bt_a2dp_sink_sbc_stop(bt_media_handle_t *handle)
{
    bt_codec_media_status_t result;
    TASK_LOG_CTRL("[SBC]stop--state: %d\r\n", handle->state);
    if (handle->state == BT_CODEC_STATE_READY) {
        handle->state = BT_CODEC_STATE_STOP;
        result = BT_CODEC_MEDIA_STATUS_OK;
    } else if (handle->state != BT_CODEC_STATE_PLAY && handle->state != BT_CODEC_STATE_ERROR) {
        result = BT_CODEC_MEDIA_STATUS_ERROR;
    } else {
        result = bt_close_sink_sbc_codec(handle);
    }
    return result;
}

static bt_codec_media_status_t bt_a2dp_sink_sbc_process(bt_media_handle_t *handle, bt_codec_media_event_t event)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    if (internal_handle == NULL) {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_a2dp_sink_parse_sbc_info(bt_a2dp_audio_internal_handle_t *internal_handle)
{
    uint8_t channel_mode, sample_rate;
    bt_codec_a2dp_audio_t *pParam = (bt_codec_a2dp_audio_t *)&internal_handle->codec_info;

    channel_mode = pParam->codec_cap.codec.sbc.channel_mode;
    sample_rate  = pParam->codec_cap.codec.sbc.sample_rate;
    TASK_LOG_I("[A2DP][SBC]sample rate=%d, channel=%d \n", sample_rate, channel_mode);
    switch (channel_mode) {
        case 8:
            internal_handle->channel_number = 1;
            break;
        case 4:
        case 2:
        case 1:
            internal_handle->channel_number = 2;
            break;
        default:
            return BT_CODEC_MEDIA_STATUS_INVALID_PARAM;
    }
    hal_audio_set_stream_out_channel_number((internal_handle->channel_number == 1) ? HAL_AUDIO_MONO : HAL_AUDIO_STEREO);

    switch (sample_rate) {
        case 8:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_16KHZ;
            break;
        case 4:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_32KHZ;
            break;
        case 2:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_44_1KHZ;
            break;
        case 1:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
            break;
        default:
            return BT_CODEC_MEDIA_STATUS_INVALID_PARAM;
    }
    hal_audio_set_stream_out_sampling_rate((hal_audio_sampling_rate_t)internal_handle->sample_rate);

    return BT_CODEC_MEDIA_STATUS_OK;
}

#ifdef MTK_BT_A2DP_AAC_ENABLE
/* aac */
static void aac_write_silence(bt_a2dp_audio_internal_handle_t *internal_handle)
{
    ring_buffer_information_t *p_ring = &internal_handle->ring_info;
    uint16_t bs_addr;
    uint16_t bs_wptr;
    uint16_t bs_rptr;

    bs_addr = *DSP_AAC_DEC_DM_BS_ADDR;
    bs_wptr = *DSP_AAC_DEC_DM_BS_MCU_W_PTR;    // in word
    bs_rptr = *DSP_AAC_DEC_DM_BS_DSP_R_PTR;    // in word
    p_ring->write_pointer       = (uint32_t)((bs_wptr - bs_addr) << 1); // in byte
    p_ring->read_pointer        = (uint32_t)((bs_rptr - bs_addr) << 1);

    {
        uint32_t loop_idx;
        uint32_t loop_cnt = 2;
        int32_t read_byte_cnt = SILENCE_TOTAL_LENGTH;
        for (loop_idx = 0; loop_idx < loop_cnt; loop_idx++) {
            uint32_t           write_byte_cnt = 0;
            uint32_t           move_byte_cnt;
            uint8_t            *p_mcu_buf      = internal_handle->aac_silence_pattern;
            volatile uint16_t  *p_dsp_buf      = NULL;

            p_mcu_buf += (SILENCE_TOTAL_LENGTH - read_byte_cnt);
            ring_buffer_get_write_information_non_mirroring(p_ring, (uint8_t **)&p_dsp_buf, &write_byte_cnt);

            write_byte_cnt &= ~0x1;     // Make it even
            move_byte_cnt = MINIMUM(write_byte_cnt, read_byte_cnt);
            {
                // Move data
                uint32_t move_word_cnt = move_byte_cnt >> 1;
                if (move_word_cnt > 0) {
                    audio_idma_write_to_dsp(p_dsp_buf, (uint16_t *)p_mcu_buf, move_word_cnt);
                    read_byte_cnt -= (move_word_cnt << 1);
                } else {    // Read buffer empty or write buffer full
                    break;
                }
            }
            ring_buffer_write_done_non_mirroring(p_ring, move_byte_cnt);
            *DSP_AAC_DEC_DM_BS_MCU_W_PTR = (uint16_t)(p_ring->write_pointer >> 1) + bs_addr;
        }
    }
    return;
}

static void aac_decoder_isr_handler(void *data)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)data;
    bt_media_handle_t *handle = (bt_media_handle_t *)internal_handle;

#ifdef BT_A2DP_BITSTREAM_DUMP_DEBUG
    TASK_LOG_I("[A2DP[AAC]ISR");
#endif

    if ((*DSP_AAC_DEC_FSM == DSP_AAC_STATE_IDLE) || (handle->state == BT_CODEC_STATE_ERROR)) {
        return;
    }

    /* error handling, but bypass the buffer underflow warning from DSP */
    if ((*DSP_AAC_DEC_ERROR_REPORT != DSP_AAC_REPORT_NONE)
            && (*DSP_AAC_DEC_ERROR_REPORT != DSP_AAC_REPORT_UNDERFLOW)) {
        internal_handle->error_count ++;

        /* fill silence when underflow continuously */
        if (*DSP_AAC_DEC_ERROR_REPORT == DSP_AAC_REPORT_BUFFER_NOT_ENOUGH) {

            int32_t mcu_data_count;
            if (handle->get_data_count_function) {
                mcu_data_count = handle->get_data_count_function();
            } else {
                mcu_data_count = handle->get_data_count(handle);
            }

            if (mcu_data_count < AAC_FILL_SILENCE_TRHESHOLD) {
                aac_write_silence(internal_handle);
            }
        } else {
            LISR_LOG_E("[A2DP]DECODER ERR(%x), FSM:%x REPORT=%x\n", (unsigned int)internal_handle->error_count, *DSP_AAC_DEC_FSM, *DSP_AAC_DEC_ERROR_REPORT);
        }

        LISR_LOG_E("[A2DP][AAC]DECODER ERR, FSM:%x  REPORT=%x\n", *DSP_AAC_DEC_FSM, *DSP_AAC_DEC_ERROR_REPORT);
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
        if (internal_handle->aws_internal_flag) {
            LISR_LOG_E("[A2DP][AAC]DECODER ERR AWS\n");
            *DSP_AAC_DEC_FSM = DSP_AAC_STATE_STOP;
            handle->state = BT_CODEC_STATE_ERROR;
            handle->handler(handle, BT_CODEC_MEDIA_ERROR);
            return;
        }
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
        if (internal_handle->error_count >= AAC_ERROR_FRAME_THRESHOLD) {
            LISR_LOG_E("[A2DP][AAC]DECODER ERR OVER THRESHOLD\n");
            *DSP_AAC_DEC_FSM = DSP_AAC_STATE_STOP;
            handle->state = BT_CODEC_STATE_ERROR;
            handle->handler(handle, BT_CODEC_MEDIA_ERROR);
            return;
        }
    } else { //if error is not consecutive, reset to 0
        internal_handle->error_count = 0;
    }

    /* bitstream buffer initialization */
    if (!internal_handle->ring_info.buffer_base_pointer) {
        uint16_t bs_page = DSP_AAC_PAGE_NUM;
        uint16_t bs_addr = *DSP_AAC_DEC_DM_BS_ADDR;
        uint16_t bs_size = *DSP_AAC_DEC_DM_BS_LEN << 1;
        internal_handle->ring_info.buffer_byte_count = (uint32_t)bs_size;
        internal_handle->ring_info.buffer_base_pointer = (uint8_t *)DSP_DM_ADDR(bs_page, bs_addr);
    }
    internal_handle->frame_count++;
    if (internal_handle->frame_count == 1) {
        uint16_t bs_addr = *DSP_AAC_DEC_DM_BS_ADDR;
        *DSP_AAC_DEC_DM_BS_MCU_W_PTR = bs_addr;
    }
    /* fill bitstream */
    bt_write_bs_to_dsp(internal_handle);
    if (handle->directly_access_dsp_function == NULL) {//Share buffer flow
        if (!handle->waiting) {
            handle->waiting = true;
            if ((!handle->underflow) && (*DSP_AAC_DEC_ERROR_REPORT == DSP_AAC_REPORT_UNDERFLOW)) {
                handle->underflow = true;
                LISR_LOG_I("[A2DP][AAC]DSP underflow \n");
                handle->handler(handle, BT_CODEC_MEDIA_UNDERFLOW);
            } else {
                handle->handler(handle, BT_CODEC_MEDIA_REQUEST);
            }
        }
    }
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    else {
        if (*DSP_AAC_DEC_ERROR_REPORT == DSP_AAC_REPORT_UNDERFLOW) {
            if (internal_handle->aws_internal_flag) {
                LISR_LOG_I("[A2DP][AAC]DSP underflow \n");
                handle->handler(handle, BT_CODEC_MEDIA_UNDERFLOW);
            }
        }
    }
#endif
}

static bt_codec_media_status_t bt_open_sink_aac_codec(bt_media_handle_t *handle)
{
    uint16_t I = 0;
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;

    TASK_LOG_CTRL("[AAC]open_codec\r\n");
    internal_handle->error_count = 0;

    audio_service_hook_isr(DSP_D2C_AAC_DEC_INT, aac_decoder_isr_handler, internal_handle);
    audio_service_setflag(handle->audio_id);
    *DSP_AUDIO_ASP_COMMON_FLAG_1 = 0;    //clear dsp audio common flag

    *DSP_AAC_DEC_ALLERROR_REPORT = 0;
    *DSP_AAC_DEC_DUAL_SCE = 0;

    if (*DSP_AAC_DEC_FSM != DSP_AAC_STATE_IDLE) {
        TASK_LOG_E("[A2DP]AAC OPEN STATE ERROR(%x)\n", *DSP_AAC_DEC_FSM);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
#if defined(AWS_DEBUG_CODE)
    TASK_LOG_I("[AWS] set AWS flag\r\n");
    bt_codec_a2dp_aws_set_flag(handle, true);
#endif  /* defined(AWS_DEBUG_CODE) */
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    if (internal_handle->aws_flag) {
        bt_codec_a2dp_aws_play_setting(internal_handle);
    }
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    internal_handle->frame_count = 0;
    *DSP_AAC_DEC_FSM = DSP_AAC_STATE_START;

    *DSP_AUDIO_CTRL2 |= DSP_AAC_CTRL_ASP;
    *DSP_AUDIO_FLEXI_CTRL |= (FLEXI_VBI_ENABLE | FLEXI_SD_ENABLE);
    afe_set_path_type(HAL_AUDIO_PLAYBACK_MUSIC);
    dsp_audio_fw_dynamic_download(DDID_AAC);

    aac_get_silence_pattern(internal_handle->sample_rate, internal_handle->channel_number, internal_handle->aac_silence_pattern);
    audio_playback_on(ASP_TYPE_AAC_DEC, internal_handle->sample_rate);

#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    if (internal_handle->aws_internal_flag == false)
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    {
        for (I = 0; ; I++) {
            if (*DSP_AAC_DEC_FSM == DSP_AAC_STATE_PLAYING) {
                break;
            }
            /* This is the case when AAC codec has started and encoutered an error,
               aacPlaybackHisr found this and set the state to STOP, then AAC codec
               set the state to IDLE. */
            if (*DSP_AAC_DEC_FSM == DSP_AAC_STATE_IDLE) {
                TASK_LOG_E("[A2DP]ERROR when AAC CODEC STARTS \n");
                break;
            }
            if (I > 80) {
                TASK_LOG_E("[A2DP][AAC] CODEC OPEN ERROR\n");
                return BT_CODEC_MEDIA_STATUS_ERROR;
            }
            hal_gpt_delay_ms(9);
        }
    }
#if defined(AWS_DEBUG_CODE)
    else {  /* Manually reset DSP for unit test */
        bt_codec_a2dp_aws_set_initial_sync(handle);
        for (I = 0; ; I++) {
            if (*DSP_AAC_DEC_FSM == DSP_AAC_STATE_PLAYING) {
                break;
            }
            /* This is the case when AAC codec has started and encoutered an error,
               aacPlaybackHisr found this and set the state to STOP, then AAC codec
               set the state to IDLE. */
            if (*DSP_AAC_DEC_FSM == DSP_AAC_STATE_IDLE) {
                TASK_LOG_E("[A2DP]ERROR when AAC CODEC STARTS \n");
                break;
            }
            if (I > 80) {
                TASK_LOG_E("[A2DP][AAC] CODEC OPEN ERROR\n");
                return BT_CODEC_MEDIA_STATUS_ERROR;
            }
            hal_gpt_delay_ms(9);
        }
    }
#endif  /* defined(AWS_DEBUG_CODE) */
    handle->state = BT_CODEC_STATE_PLAY;
    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_close_sink_aac_codec(bt_media_handle_t *handle)
{
    uint16_t I = 0;
    TASK_LOG_CTRL("[AAC]close_codec\r\n");
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    {
        bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)handle;
        if (internal_handle->aws_internal_flag && !internal_handle->aws_flag && !internal_handle->aws_init_sync_flag) {
            *DSP_AAC_DEC_FSM = DSP_AAC_STATE_IDLE;
        }
    }
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    for (I = 0; ; I++) {
        if (*DSP_AAC_DEC_FSM == DSP_AAC_STATE_IDLE) {
            break;
        }
        if (*DSP_AAC_DEC_FSM == DSP_AAC_STATE_PLAYING) {
            *DSP_AAC_DEC_FSM = DSP_AAC_STATE_STOP;
        }
        if (I > 80) {
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
        hal_gpt_delay_ms(9);
    }

    *DSP_AUDIO_CTRL2 &= ~(DSP_AAC_CTRL_ASP | DSP_PCM_R_DIS);
    *DSP_AUDIO_FLEXI_CTRL &= ~(FLEXI_VBI_ENABLE | FLEXI_SD_ENABLE);

    audio_playback_off();
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    {
        bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)handle;
        if (internal_handle->aws_internal_flag) {
            bt_codec_a2dp_aws_stop_setting(internal_handle);
        }
    }
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
#if defined(AWS_DEBUG_CODE)
    TASK_LOG_I("[AWS] clear AWS flag\r\n");
    bt_codec_a2dp_aws_set_flag(handle, false);
#endif  /* defined(AWS_DEBUG_CODE) */
    audio_service_unhook_isr(DSP_D2C_AAC_DEC_INT);
    audio_service_clearflag(handle->audio_id);
    handle->state = BT_CODEC_STATE_STOP;
    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_a2dp_sink_aac_play(bt_media_handle_t *handle)
{
    TASK_LOG_CTRL("[AAC]play\r\n");
    if (handle->state != BT_CODEC_STATE_READY && handle->state != BT_CODEC_STATE_STOP) {
        TASK_LOG_E("[A2DP][AAC] CODEC PLAY ERROR \n");
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
    return bt_open_sink_aac_codec(handle);
}

static bt_codec_media_status_t bt_a2dp_sink_aac_stop(bt_media_handle_t *handle)
{
    bt_codec_media_status_t result;
    TASK_LOG_CTRL("[AAC]stop--state: %d\r\n", handle->state);
    if (handle->state == BT_CODEC_STATE_READY) {
        handle->state = BT_CODEC_STATE_STOP;
        result = BT_CODEC_MEDIA_STATUS_OK;
    } else if (handle->state != BT_CODEC_STATE_PLAY && handle->state != BT_CODEC_STATE_ERROR) {
        TASK_LOG_E("[A2DP][AAC] CODEC STOP ERROR \n");
        result = BT_CODEC_MEDIA_STATUS_ERROR;
    } else {
        result = bt_close_sink_aac_codec(handle);
    }
    return result;
}

static bt_codec_media_status_t bt_a2dp_sink_aac_process(bt_media_handle_t *handle, bt_codec_media_event_t event)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    if (internal_handle == NULL) {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_a2dp_sink_parse_aac_info(bt_a2dp_audio_internal_handle_t *internal_handle)
{
    uint8_t channel_mode;
    uint16_t sample_rate;
    bt_codec_a2dp_audio_t *pParam = (bt_codec_a2dp_audio_t *)&internal_handle->codec_info;

    channel_mode = pParam->codec_cap.codec.aac.channels;
    sample_rate  = pParam->codec_cap.codec.aac.sample_rate;
    TASK_LOG_I("[A2DP][AAC] sample rate=%x, channel=%x \n", sample_rate, channel_mode);

    switch (channel_mode) {
        case 0x2:
            internal_handle->channel_number = 1;
            break;
        case 0x1:
            internal_handle->channel_number = 2;
            break;
        default:
            return BT_CODEC_MEDIA_STATUS_INVALID_PARAM;
    }
    hal_audio_set_stream_out_channel_number((internal_handle->channel_number == 1) ? HAL_AUDIO_MONO : HAL_AUDIO_STEREO);

    switch (sample_rate) {
        case 0x800:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_8KHZ;
            break;
        case 0x400:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_11_025KHZ;
            break;
        case 0x200:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_12KHZ;
            break;
        case 0x100:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_16KHZ;
            break;
        case 0x80:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_22_05KHZ;
            break;
        case 0x40:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_24KHZ;
            break;
        case 0x20:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_32KHZ;
            break;
        case 0x10:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_44_1KHZ;
            break;
        case 0x8:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
            break;
        default:
            return BT_CODEC_MEDIA_STATUS_INVALID_PARAM;
    }
    hal_audio_set_stream_out_sampling_rate((hal_audio_sampling_rate_t)internal_handle->sample_rate);

    return BT_CODEC_MEDIA_STATUS_OK;
}
#endif /*MTK_BT_A2DP_AAC_ENABLE*/

#ifdef MTK_BT_A2DP_SOURCE_SUPPORT

// a2dp source related API
static uint32_t bt_codec_a2dp_source_query_memory_size(bt_media_handle_t *handle)
{
    return sbc_codec_get_buffer_size();
}

static uint32_t bt_codec_a2dp_source_get_payload(bt_media_handle_t *handle, uint8_t *buffer, uint32_t buffer_size, uint32_t *sample_count)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    sbc_codec_media_handle_t        *encoder_handle  = internal_handle->sbc_encode_handle;
    if (encoder_handle == NULL) {
        TASK_LOG_E("[A2DP source] NULL handle;");
        return 0;
    }
    return encoder_handle->get_payload(encoder_handle, buffer, buffer_size, sample_count);
}

static void bt_codec_a2dp_source_get_payload_done(bt_media_handle_t *handle)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    sbc_codec_media_handle_t        *encoder_handle  = internal_handle->sbc_encode_handle;
    if (encoder_handle == NULL) {
        TASK_LOG_E("[A2DP source] NULL handle;");
    } else {
        encoder_handle->get_payload_done(internal_handle->sbc_encode_handle);
    }
}

static void bt_codec_a2dp_source_query_payload_size(bt_media_handle_t *handle, uint32_t *minimum, uint32_t *total)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    sbc_codec_media_handle_t        *encoder_handle  = internal_handle->sbc_encode_handle;
    if (encoder_handle == NULL) {
        TASK_LOG_E("[A2DP source] NULL handle;");
    } else {
        encoder_handle->query_payload_size(encoder_handle, minimum, total);
    }
}

void bt_codec_a2dp_source_event_callback(void *hdl, sbc_codec_event_t event)
{
    bt_media_handle_t *handle = (bt_media_handle_t *)hdl;

    switch (event) {
        case SBC_CODEC_MEDIA_GET_PAYLOAD: {
            handle->handler(handle, BT_CODEC_MEDIA_GET_PAYLOAD);
        }
        break;
    }
}

static uint32_t bt_codec_a2dp_source_set_bit_rate(bt_media_handle_t *handle, uint32_t bit_rate)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    sbc_codec_media_handle_t *encoder_handle = NULL;
    if (internal_handle->sbc_encode_handle == NULL) {
        encoder_handle = sbc_codec_open(
                             bt_codec_a2dp_source_event_callback, &(internal_handle->initial_parameter), handle);
        internal_handle->sbc_encode_handle = encoder_handle;
    } else {
        encoder_handle = internal_handle->sbc_encode_handle;
    }

    return encoder_handle->set_bit_rate(encoder_handle, bit_rate);
}

static bt_codec_media_status_t bt_open_source_sbc_codec(bt_media_handle_t *handle)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    sbc_codec_media_handle_t *encoder_handle = NULL;
    if (internal_handle->sbc_encode_handle == NULL) {
        encoder_handle = sbc_codec_open(
                             bt_codec_a2dp_source_event_callback, &(internal_handle->initial_parameter), handle);
        internal_handle->sbc_encode_handle = encoder_handle;
    } else {
        encoder_handle = internal_handle->sbc_encode_handle;
    }

    if (encoder_handle == NULL) {
        TASK_LOG_E("[A2DP source][SBC] sbc encoder init fail.");
        handle->state = BT_CODEC_STATE_ERROR;
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    if (handle->buffer_info.buffer_base == NULL) {
        TASK_LOG_E("[A2DP source][SBC] set buffer before play.");
        handle->state = BT_CODEC_STATE_ERROR;
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    encoder_handle->set_buffer(encoder_handle, handle->buffer_info.buffer_base, handle->buffer_info.buffer_size);
    encoder_handle->play(encoder_handle);

    handle->state = BT_CODEC_STATE_PLAY;
    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_close_source_sbc_codec(bt_media_handle_t *handle)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    sbc_codec_media_handle_t *encoder_handle = internal_handle->sbc_encode_handle;

    if (encoder_handle == NULL) {
        TASK_LOG_E("[SBC] NULL handle\r\n");
        handle->state = BT_CODEC_STATE_ERROR;
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    if (encoder_handle->stop(encoder_handle) < 0) {
        TASK_LOG_E("[A2DP source][SBC] sbc encoder stop fail.");
        handle->state = BT_CODEC_STATE_ERROR;
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    if (sbc_codec_close(encoder_handle) < 0) {
        TASK_LOG_E("[A2DP source][SBC] sbc encoder close fail.");
        handle->state = BT_CODEC_STATE_ERROR;
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
    internal_handle->sbc_encode_handle = NULL;

    handle->state = BT_CODEC_STATE_STOP;
    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_a2dp_source_sbc_play(bt_media_handle_t *handle)
{
    if (handle->state != BT_CODEC_STATE_READY && handle->state != BT_CODEC_STATE_STOP) {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
#if defined(BT_A2DP_BITSTREAM_DUMP_DEBUG)
    bt_a2dp_ptr = 0;
    memset(bt_a2dp_bitstream, 0, BT_A2DP_BS_LEN * sizeof(uint8_t));
#endif /* BT_A2DP_BITSTREAM_DUMP_DEBUG */
    return bt_open_source_sbc_codec(handle);
}

static bt_codec_media_status_t bt_a2dp_source_sbc_stop(bt_media_handle_t *handle)
{
    bt_codec_media_status_t result;
    if (handle->state == BT_CODEC_STATE_READY) {
        handle->state = BT_CODEC_STATE_STOP;
        result = BT_CODEC_MEDIA_STATUS_OK;
    } else if (handle->state != BT_CODEC_STATE_PLAY && handle->state != BT_CODEC_STATE_ERROR) {
        result = BT_CODEC_MEDIA_STATUS_ERROR;
    } else {
        result = bt_close_source_sbc_codec(handle);
    }
    return result;
}

static bt_codec_media_status_t bt_a2dp_source_sbc_process(bt_media_handle_t *handle, bt_codec_media_event_t event)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    if (internal_handle == NULL) {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_a2dp_source_parse_sbc_info(bt_a2dp_audio_internal_handle_t *internal_handle)
{
    sbc_codec_initial_parameter_t *pDst = (sbc_codec_initial_parameter_t *)&internal_handle->initial_parameter;

    pDst->role = SBC_CODEC_MEDIA_ENCODER;
    pDst->cap.alloc_method = internal_handle->codec_info.codec_cap.codec.sbc.alloc_method;
    pDst->cap.block_length = internal_handle->codec_info.codec_cap.codec.sbc.block_length;
    pDst->cap.channel_mode = internal_handle->codec_info.codec_cap.codec.sbc.channel_mode;
    pDst->cap.max_bit_pool = internal_handle->codec_info.codec_cap.codec.sbc.max_bit_pool;
    pDst->cap.min_bit_pool = internal_handle->codec_info.codec_cap.codec.sbc.min_bit_pool;
    pDst->cap.sample_rate  = internal_handle->codec_info.codec_cap.codec.sbc.sample_rate;
    pDst->cap.subband_num  = internal_handle->codec_info.codec_cap.codec.sbc.subband_num;

    return BT_CODEC_MEDIA_STATUS_OK;
}

#endif /*MTK_BT_A2DP_SOURCE_SUPPORT*/

bt_media_handle_t *bt_codec_a2dp_open(bt_codec_a2dp_callback_t bt_a2dp_callback , const bt_codec_a2dp_audio_t *param)
{
    bt_media_handle_t *handle;
    bt_a2dp_audio_internal_handle_t *internal_handle = NULL; /*internal handler*/
    uint16_t audio_id = audio_get_id();
    TASK_LOG_I("[A2DP]Open codec\n");
    if (audio_id > MAX_AUDIO_FUNCTIONS) {
        return 0;
    }
#ifdef BT_A2DP_BITSTREAM_DUMP_DEBUG
    TASK_LOG_I("[A2DP]Open codec--role: %d, type: %d, 1: %d, 2: %d, 3: %d, 4: %d, 5: %d, 6: %d, 7: %d\n",
               param->role, param->codec_cap.type,
               param->codec_cap.codec.sbc.alloc_method,
               param->codec_cap.codec.sbc.block_length,
               param->codec_cap.codec.sbc.channel_mode,
               param->codec_cap.codec.sbc.max_bit_pool,
               param->codec_cap.codec.sbc.min_bit_pool,
               param->codec_cap.codec.sbc.sample_rate,
               param->codec_cap.codec.sbc.subband_num);
#endif
    /* alloc internal handle space */
    internal_handle = (bt_a2dp_audio_internal_handle_t *)pvPortMalloc(sizeof(bt_a2dp_audio_internal_handle_t));
    if (internal_handle == NULL) {
        return 0;
    }

    memset(internal_handle, 0, sizeof(bt_a2dp_audio_internal_handle_t));
    handle = &internal_handle->handle;
    internal_handle->codec_info = *(bt_codec_a2dp_audio_t *)param;
    handle->audio_id = audio_id;
    handle->handler = bt_a2dp_callback;
    handle->directly_access_dsp_function = NULL;
    handle->get_data_count_function = NULL;
    handle->buffer_info.buffer_base = NULL;
    bt_codec_buffer_function_init(handle);
    if (internal_handle->codec_info.role == BT_A2DP_SINK) {
        if (internal_handle->codec_info.codec_cap.type == BT_A2DP_CODEC_SBC) {
            bt_codec_media_status_t result = BT_CODEC_MEDIA_STATUS_OK;
            handle->play    = bt_a2dp_sink_sbc_play;
            handle->stop    = bt_a2dp_sink_sbc_stop;
            handle->process = bt_a2dp_sink_sbc_process;
            result = bt_a2dp_sink_parse_sbc_info(internal_handle);
            if (BT_CODEC_MEDIA_STATUS_OK != result) {
                if (internal_handle != NULL) {
                    vPortFree(internal_handle);
                    internal_handle = NULL;
                }
                return 0;
            }
#ifdef BT_A2DP_BITSTREAM_DUMP_DEBUG
            TASK_LOG_I("[A2DP][SBC]Codec open");
#endif
        }
#ifdef MTK_BT_A2DP_AAC_ENABLE
        else if (internal_handle->codec_info.codec_cap.type == BT_A2DP_CODEC_AAC) {
            bt_codec_media_status_t result = BT_CODEC_MEDIA_STATUS_OK;
            handle->play    = bt_a2dp_sink_aac_play;
            handle->stop    = bt_a2dp_sink_aac_stop;
            handle->process = bt_a2dp_sink_aac_process;
            result = bt_a2dp_sink_parse_aac_info(internal_handle);
            if (BT_CODEC_MEDIA_STATUS_OK != result) {
                if (internal_handle != NULL) {
                    vPortFree(internal_handle);
                    internal_handle = NULL;
                }
                return 0;
            }
#ifdef BT_A2DP_BITSTREAM_DUMP_DEBUG
            TASK_LOG_I("[A2DP[AAC]Codec open");
#endif
        }
#endif /*MTK_BT_A2DP_AAC_ENABLE*/
        else {
            if (internal_handle != NULL) {
                vPortFree(internal_handle);
                internal_handle = NULL;
            }
            return 0;
        }
    } else {
#ifdef MTK_BT_A2DP_SOURCE_SUPPORT
        /* A2DP source role */
        if (internal_handle->codec_info.codec_cap.type == BT_A2DP_CODEC_SBC) {
            bt_codec_media_status_t result = BT_CODEC_MEDIA_STATUS_OK;
            handle->play    = bt_a2dp_source_sbc_play;
            handle->stop    = bt_a2dp_source_sbc_stop;
            handle->process = bt_a2dp_source_sbc_process;

            handle->query_memory_size  = bt_codec_a2dp_source_query_memory_size;
            handle->set_bit_rate       = bt_codec_a2dp_source_set_bit_rate;
            handle->get_payload        = bt_codec_a2dp_source_get_payload;
            handle->get_payload_done   = bt_codec_a2dp_source_get_payload_done;
            handle->query_payload_size = bt_codec_a2dp_source_query_payload_size;
            result = bt_a2dp_source_parse_sbc_info(internal_handle);
            if (BT_CODEC_MEDIA_STATUS_OK != result) {
                if (internal_handle != NULL) {
                    vPortFree(internal_handle);
                    internal_handle = NULL;
                }
                return 0;
            }
        } else {
            if (internal_handle != NULL) {
                vPortFree(internal_handle);
                internal_handle = NULL;
            }
            return 0;
        }
#endif /*MTK_BT_A2DP_SOURCE_SUPPORT*/
    }
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    {
        int32_t result = bt_codec_a2dp_aws_open_setting(internal_handle);
        if (result < 0) {
            TASK_LOG_I("[A2DP][AWS]alloc fail, result = %d\r\n", (int)result);
            if (internal_handle != NULL) {
                vPortFree(internal_handle);
                internal_handle = NULL;
            }
            return 0;
        }
    }
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    handle->state = BT_CODEC_STATE_READY;
    return handle;
}

bt_codec_media_status_t bt_codec_a2dp_close(bt_media_handle_t *handle)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;

    TASK_LOG_I("[A2DP]Close codec\n");
    if (handle->state != BT_CODEC_STATE_STOP && handle->state != BT_CODEC_STATE_READY) {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
    handle->state = BT_CODEC_STATE_IDLE;
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    bt_codec_a2dp_aws_close_setting(internal_handle);
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    audio_free_id(handle->audio_id);
    if (internal_handle != NULL) {
        vPortFree(internal_handle);
        internal_handle = NULL;
    }
    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_codec_media_status_t bt_codec_a2dp_aws_set_clock_skew(bool flag)
{
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    if (flag) {
        audio_service_aws_set_clock_skew(true);
    } else {
        audio_service_aws_set_clock_skew(false);
    }
    return BT_CODEC_MEDIA_STATUS_OK;
#else   /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
}
bt_codec_media_status_t bt_codec_a2dp_aws_set_flag(bt_media_handle_t *handle, bool flag)
{
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    bt_a2dp_audio_internal_handle_t *p_info = (bt_a2dp_audio_internal_handle_t *)handle;
    p_info->aws_flag = flag;
    return BT_CODEC_MEDIA_STATUS_OK;
#else   /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
}

bt_codec_media_status_t bt_codec_a2dp_aws_set_initial_sync(bt_media_handle_t *handle)
{
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    bt_codec_media_status_t result = BT_CODEC_MEDIA_STATUS_OK;
    bt_a2dp_audio_internal_handle_t *p_info = (bt_a2dp_audio_internal_handle_t *)handle;
    if (handle != NULL && p_info->aws_internal_flag) {
        p_info->aws_init_sync_flag = true;
        audio_service_aws_set_initial_sync();
    } else {
        result = BT_CODEC_MEDIA_STATUS_ERROR;
    }
    return result;
#else   /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
}

bt_codec_media_status_t bt_codec_a2dp_aws_get_consumed_data_count(bt_media_handle_t *handle, bt_codec_a2dp_data_count_t *information)
{
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    bt_a2dp_audio_internal_handle_t *p_info = (bt_a2dp_audio_internal_handle_t *)handle;
    information->sample_count  = audio_service_aws_get_accumulated_sample_count();
    information->sampling_rate = bt_codec_a2dp_aws_convert_sampling_rate_from_index_to_value(p_info->sample_rate);
    return BT_CODEC_MEDIA_STATUS_OK;
#else   /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
}

bt_codec_media_status_t bt_codec_a2dp_aws_get_silence_frame_information(bt_media_handle_t *handle, bt_codec_a2dp_bitstream_t *information)
{
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    bt_codec_media_status_t result = BT_CODEC_MEDIA_STATUS_OK;
    bt_a2dp_audio_internal_handle_t *p_info = (bt_a2dp_audio_internal_handle_t *)handle;
    bt_a2dp_codec_type_t codec_type = p_info->codec_info.codec_cap.type;
    uint32_t *sil_smpl_count = &information->sample_count;
    uint32_t *sil_byte_count = &information->byte_count;
    if (codec_type == BT_A2DP_CODEC_SBC) {
        bt_codec_sbc_t *p_codec_info = &p_info->codec_info.codec_cap.codec.sbc;
        sbc_set_silence_pattern_frame_sample(p_codec_info->block_length, p_codec_info->subband_num);
        audio_service_aws_get_silence_frame_information(AWS_CODEC_TYPE_SBC_FORMAT, sil_smpl_count, sil_byte_count);
    } else if (codec_type == BT_A2DP_CODEC_AAC) {
#ifdef MTK_BT_A2DP_AAC_ENABLE
        audio_service_aws_get_silence_frame_information(AWS_CODEC_TYPE_AAC_FORMAT, sil_smpl_count, sil_byte_count);
#else   /* MTK_BT_A2DP_AAC_ENABLE */
        information->sample_count = 0;
        information->byte_count = 0;
        result = BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* MTK_BT_A2DP_AAC_ENABLE */
    } else {
        information->sample_count = 0;
        information->byte_count = 0;
        result = BT_CODEC_MEDIA_STATUS_ERROR;
    }
    return result;
#else   /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
}

bt_codec_media_status_t bt_codec_a2dp_aws_fill_silence_frame(bt_media_handle_t *handle, bt_codec_a2dp_buffer_t *data, uint32_t target_frm_cnt)
{
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    int32_t result;
    uint32_t *sil_frm_cnt = &target_frm_cnt;
    uint32_t *buf_byte_cnt = &data->byte_count;
    bt_a2dp_audio_internal_handle_t *p_info = (bt_a2dp_audio_internal_handle_t *)handle;
    bt_a2dp_codec_type_t codec_type = p_info->codec_info.codec_cap.type;
    if (codec_type == BT_A2DP_CODEC_SBC) {
        bt_codec_sbc_t *p_codec_info = &p_info->codec_info.codec_cap.codec.sbc;
        sbc_set_silence_pattern_frame_sample(p_codec_info->block_length, p_codec_info->subband_num);
        result = audio_service_aws_fill_silence_frame(data->buffer, buf_byte_cnt, AWS_CODEC_TYPE_SBC_FORMAT, sil_frm_cnt);
        if (result == HAL_AUDIO_AWS_ERROR || result == HAL_AUDIO_AWS_NOT_SUPPORT) {
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
    } else if (codec_type == BT_A2DP_CODEC_AAC) {
#ifdef MTK_BT_A2DP_AAC_ENABLE
        result = audio_service_aws_fill_silence_frame(data->buffer, buf_byte_cnt, AWS_CODEC_TYPE_AAC_FORMAT, sil_frm_cnt);
        if (result == HAL_AUDIO_AWS_ERROR || result == HAL_AUDIO_AWS_NOT_SUPPORT) {
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
#else   /* MTK_BT_A2DP_AAC_ENABLE */
        return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* MTK_BT_A2DP_AAC_ENABLE */
    } else {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
    return BT_CODEC_MEDIA_STATUS_OK;
#else   /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
}

bt_codec_media_status_t bt_codec_a2dp_aws_parse_data_information(bt_media_handle_t *handle, bt_codec_a2dp_buffer_t *data, bt_codec_a2dp_bitstream_t *information)
{
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    int32_t result;
    bt_a2dp_audio_internal_handle_t *p_info = (bt_a2dp_audio_internal_handle_t *)handle;
    bt_a2dp_codec_type_t codec_type = p_info->codec_info.codec_cap.type;
    uint32_t *out_smpl_cnt = &information->sample_count;
    uint32_t *out_byte_cnt = &information->byte_count;
    if (codec_type == BT_A2DP_CODEC_SBC) {
        result = audio_service_aws_parse_bitstream_information(data->buffer, data->byte_count, AWS_CODEC_TYPE_SBC_FORMAT, out_smpl_cnt, out_byte_cnt);
        if (result == HAL_AUDIO_AWS_ERROR || result == HAL_AUDIO_AWS_NOT_SUPPORT) {
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
    } else if (codec_type == BT_A2DP_CODEC_AAC) {
#ifdef MTK_BT_A2DP_AAC_ENABLE
        result = audio_service_aws_parse_bitstream_information(data->buffer, data->byte_count, AWS_CODEC_TYPE_AAC_FORMAT, out_smpl_cnt, out_byte_cnt);
        if (result == HAL_AUDIO_AWS_ERROR || result == HAL_AUDIO_AWS_NOT_SUPPORT) {
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
#else   /* MTK_BT_A2DP_AAC_ENABLE */
        information->sample_count = 0;
        information->byte_count = 0;
        return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* MTK_BT_A2DP_AAC_ENABLE */
    } else {
        information->sample_count = 0;
        information->byte_count = 0;
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
    return BT_CODEC_MEDIA_STATUS_OK;
#else   /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
}

bt_codec_media_status_t bt_codec_a2dp_aws_get_clock_skew_status(bt_media_handle_t *handle, bt_codec_aws_clock_skew_status_t *status)
{
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    aws_clock_skew_status_t hal_aws_status;
    hal_aws_status = audio_service_aws_get_clock_skew_status();
    if (hal_aws_status == AWS_CLOCK_SKEW_STATUS_IDLE) {
        *status = BT_CODEC_AWS_CLOCK_SKEW_STATUS_IDLE;
    } else if (hal_aws_status == AWS_CLOCK_SKEW_STATUS_BUSY) {
        *status = BT_CODEC_AWS_CLOCK_SKEW_STATUS_BUSY;
    }
    return BT_CODEC_MEDIA_STATUS_OK;
#else   /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
}

bt_codec_media_status_t bt_codec_a2dp_aws_set_clock_skew_compensation_value(bt_media_handle_t *handle, int32_t sample_count)
{
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    int result;
    result = audio_service_aws_set_clock_skew_compensation_value(sample_count);
    if (result < 0) {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
    return BT_CODEC_MEDIA_STATUS_OK;
#else   /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
}

bt_codec_media_status_t bt_codec_a2dp_aws_get_audio_latency(bt_media_handle_t *handle, uint32_t sampling_rate, uint32_t *p_latency_us)
{
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    bt_codec_media_status_t result = BT_CODEC_MEDIA_STATUS_OK;
    bt_a2dp_audio_internal_handle_t *p_info = (bt_a2dp_audio_internal_handle_t *)handle;
    bt_a2dp_codec_type_t codec_type = p_info->codec_info.codec_cap.type;
    uint32_t latency_us;
    if (codec_type == BT_A2DP_CODEC_SBC) {
        switch (sampling_rate) {
            case 48000:
                latency_us = 4805;
                break;
            case 44100:
                latency_us = 4805;
                break;
            case 32000:
                latency_us = 4805;
                break;
            case 16000:
                latency_us = 4805;
                break;
            default:
                latency_us = 0;
                result = BT_CODEC_MEDIA_STATUS_ERROR;
                break;
        }
    } else if (codec_type == BT_A2DP_CODEC_AAC) {
#ifdef MTK_BT_A2DP_AAC_ENABLE
        switch (sampling_rate) {
            case 48000:
                latency_us = 8604;
                break;
            case 44100:
                latency_us = 8604;
                break;
            case 32000:
                latency_us = 8604;
                break;
            case 24000:
                latency_us = 8604;
                break;
            case 22050:
                latency_us = 8604;
                break;
            case 16000:
                latency_us = 8604;
                break;
            case 12000:
                latency_us = 8604;
                break;
            case 11025:
                latency_us = 8604;
                break;
            case  8000:
                latency_us = 8604;
                break;
            default:
                latency_us = 0;
                result = BT_CODEC_MEDIA_STATUS_ERROR;
                break;
        }
#else   /* MTK_BT_A2DP_AAC_ENABLE */
        latency_us = 0;
        result = BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* MTK_BT_A2DP_AAC_ENABLE */
    } else {
        latency_us = 0;
        result = BT_CODEC_MEDIA_STATUS_ERROR;
    }
    *p_latency_us = latency_us;
    return result;
#else   /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
}


void bt_codec_a2dp_set_sw_aac(bool flag)
{
    return;
}

bt_codec_media_status_t bt_codec_a2dp_set_sw_aac_flag(bool flag)
{
    return BT_CODEC_MEDIA_STATUS_OK;
}

