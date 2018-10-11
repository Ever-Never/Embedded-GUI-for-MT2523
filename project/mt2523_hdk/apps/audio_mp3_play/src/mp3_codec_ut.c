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
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
//#include "debug_interaction.h"
#include "FreeRTOS.h"
#include "task.h"
#include "mp3_codec_ut.h"
#include "mp3_codec.h"
#include "hal_audio.h"
#include "hal_log.h"
#include "ff.h"


static FATFS fatfs;
static FIL fdst;
static FRESULT res;
static UINT length_read;
static uint8_t first_time_f_mount = 1;

static int16_t test_open_file_from_sd(FIL *fp, const TCHAR *path)
{
    FRESULT res;

    if (first_time_f_mount){
        res = f_mount(&fatfs, _T("0:"), 1);
        first_time_f_mount = 0;
    } else {
        log_hal_info("already fmount\n");
        res = FR_OK;
    }

    if (!res) {
        log_hal_info("fmount ok \n");
        res = f_open(fp, path, FA_OPEN_EXISTING | FA_WRITE | FA_READ);
        if (!res) {
            log_hal_info("fopen ok \n");
        } else {
            log_hal_info("fopen error \n");
            return -1;
        }
    } else {
        log_hal_info("fmount error \n");
    }

    if (res != FR_OK) {
        return -1;
    } else {
        return 0;
    }
}

static void test_stop_read_from_sd(mp3_codec_media_handle_t *hdl)
{
    hdl->stop(hdl);
    res = f_close(&fdst);
    if (!res) {
        log_hal_info("[MP3 Codec] fclose success \n");
    } else {
        log_hal_info("[MP3 Codec] fclose error \n");
    }
}


static void test_sd_event_callback(mp3_codec_media_handle_t *hdl, mp3_codec_event_t event)
{
    uint8_t *share_buf;
    uint32_t share_buf_len;

    switch (event) {
        case MP3_CODEC_MEDIA_JUMP_FILE_TO:
            res = f_lseek(&fdst, (DWORD)hdl->jump_file_to_specified_position);
            log_hal_info("[MP3 Codec ut] MP3_CODEC_MEDIA_JUMP_FILE_TO: fdst read/write pointer=%x\n", f_tell(&fdst));

            break;
        case MP3_CODEC_MEDIA_REQUEST:
        case MP3_CODEC_MEDIA_UNDERFLOW:
            if (f_eof(&fdst)) {
                log_hal_info("[MP3 Codec ut] [EOF]End of this mp3 file. Stop mp3\n");
                test_stop_read_from_sd(hdl);
            }
            uint32_t loop_idx;
            uint32_t loop_cnt = 2;
            for (loop_idx = 0; loop_idx < loop_cnt; loop_idx++) {
                hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
                f_read(&fdst, share_buf, share_buf_len, &length_read);
                hdl->write_data_done(hdl, share_buf_len);
            }

            hdl->finish_write_data(hdl);
            break;
    }
}



void mp3_codec_ut(void *pvParameters)
{
    // This mp3 uint test test as the following
    // Put 3 mp3 file in SD card temp folder: SD:/temp/1.mp3, SD:/temp/2.mp3 and SD:/temp/3.mp3.
    // The name of the mp3 file must be the same as 1, 2 and 3.
    // It will open mp3 codect first and then play 1.mp3 then test pause and resume and then stop 1.mp3
    // After stop 1.mp3 it will change the file to 2.mp3 then test pause and resume and then stop 2.mp3 and then close mp3 codec.
    // After close mp3 codec, then it will open mp3 codec again and play 3.mp3 and then test pause and resume and then stop 3.mp3 and then close mp3 codec

    mp3_codec_media_handle_t *hdl = NULL;
    //uint32_t memory_size = 0;
    //uint8_t  *memory;//should 4 bytes aligned
    uint8_t *share_buf;
    uint32_t share_buf_len;

    // Open the mp3 codec.
    log_hal_info("[MP3 Codec ut] open mp3 codec\n");
    hdl = mp3_codec_open(test_sd_event_callback);
    if (hdl == NULL) {
        log_hal_error("[MP3 Codec ut] Fail to open the codec.");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }
/*

    mp3_codec_get_memory_size(&memory_size);

    memory = pvPortMalloc(memory_size * sizeof(uint8_t *));
    if (NULL == memory) {
        log_hal_error("[MP3 Codec] Fail to allocate the memory");
        return -1;
    } else {
        log_hal_info("[MP3 Codec] memory_size=%u\n", memory_size);
        log_hal_info("[MP3 Codec] memory_start_adr=%08x\n", memory);
        log_hal_info("[MP3 Codec] memory_end_adr=%08x\n", memory + memory_size);
        mp3_codec_set_memory(memory);
    }
*/
    mp3_codec_set_memory2();



    if (test_open_file_from_sd(&fdst, _T("SD:/temp/1.mp3")) < 0) {
        log_hal_error("[MP3 Codec ut] Fail to open SD:/temp/1.mp3.");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }

    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);


    hdl->write_data_done(hdl, share_buf_len);
    hdl->finish_write_data(hdl);


    hdl->skip_id3v2_and_reach_next_frame(hdl, (uint32_t)f_size(&fdst));

    hal_audio_set_stream_out_device(HAL_AUDIO_DEVICE_HANDS_FREE_MONO);
    hal_audio_set_stream_out_volume(0xFFFFFA88, 0x0);

    log_hal_info("[MP3 Codec ut] play\n");
    hdl->play(hdl);
    vTaskDelay(20000 / portTICK_RATE_MS);

    log_hal_info("[MP3 Codec ut] pause\n");
    hdl->pause(hdl);
    vTaskDelay(5000 / portTICK_RATE_MS);

    log_hal_info("[MP3 Codec ut] resume\n");
    hdl->resume(hdl);
    vTaskDelay(5000 / portTICK_RATE_MS);

    log_hal_info("[MP3 Codec ut] pause\n");
    hdl->pause(hdl);
    vTaskDelay(5000 / portTICK_RATE_MS);

    log_hal_info("[MP3 Codec ut] resume\n");
    hdl->resume(hdl);
    vTaskDelay(5000 / portTICK_RATE_MS);

    log_hal_info("[MP3 Codec ut] stop\n");
    test_stop_read_from_sd(hdl);



    vTaskDelay(5000 / portTICK_RATE_MS);



    log_hal_info("[MP3 Codec ut] change file\n");



    if (test_open_file_from_sd(&fdst, _T("SD:/temp/2.mp3")) < 0) {
        log_hal_error("[MP3 Codec ut] Fail to open SD:/temp/2.mp3.\n");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }

    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);


    hdl->write_data_done(hdl, share_buf_len);
    hdl->finish_write_data(hdl);


    hdl->skip_id3v2_and_reach_next_frame(hdl, (uint32_t)f_size(&fdst));

    hal_audio_set_stream_out_device(HAL_AUDIO_DEVICE_HANDS_FREE_MONO);  // piter mark: need to change to stereo
    hal_audio_set_stream_out_volume(0xFFFFFA88, 0x0);

    log_hal_info("[MP3 Codec ut] play\n");
    hdl->play(hdl);
    vTaskDelay(20000 / portTICK_RATE_MS);

    log_hal_info("[MP3 Codec ut] pause\n");
    hdl->pause(hdl);
    vTaskDelay(5000 / portTICK_RATE_MS);

    log_hal_info("[MP3 Codec ut] resume\n");
    hdl->resume(hdl);
    vTaskDelay(5000 / portTICK_RATE_MS);

    log_hal_info("[MP3 Codec ut] pause\n");
    hdl->pause(hdl);
    vTaskDelay(5000 / portTICK_RATE_MS);

    log_hal_info("[MP3 Codec ut] resume\n");
    hdl->resume(hdl);
    vTaskDelay(5000 / portTICK_RATE_MS);

    log_hal_info("[MP3 Codec ut] stop\n");
    test_stop_read_from_sd(hdl);

    log_hal_info("[MP3 Codec ut] close mp3 codec\n");
    hdl->close_codec(hdl);


    // Open the mp3 codec.
    log_hal_info("[MP3 Codec ut] open mp3 codec\n");
    hdl = mp3_codec_open(test_sd_event_callback);
    if (hdl == NULL) {
        log_hal_error("[MP3 Codec ut] Fail to open the codec.\n");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }

/*

    mp3_codec_get_memory_size(&memory_size);

    memory = pvPortMalloc(memory_size * sizeof(uint8_t *));
    if (NULL == memory) {
        log_hal_error("[MP3 Codec ut] Fail to allocate the memory");
        return -1;
    } else {
        log_hal_info("[MP3 Codec ut] memory_size=%u\n", memory_size);
        log_hal_info("[MP3 Codec ut] memory_start_adr=%08x\n", memory);
        log_hal_info("[MP3 Codec ut] memory_end_adr=%08x\n", memory + memory_size);
        mp3_codec_set_memory(memory);
    }
*/
    mp3_codec_set_memory2();



    if (test_open_file_from_sd(&fdst, _T("SD:/temp/3.mp3")) < 0) {
        log_hal_error("[MP3 Codec ut] Fail to open SD:/temp/3.mp3\n");
        while (1) {
            vTaskDelay(10 / portTICK_RATE_MS);
        }
    }


    /* prefill data to share  buffer */
    hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
    f_read(&fdst, share_buf, share_buf_len, &length_read);


    hdl->write_data_done(hdl, share_buf_len);
    hdl->finish_write_data(hdl);


    hdl->skip_id3v2_and_reach_next_frame(hdl, (uint32_t)f_size(&fdst));

    hal_audio_set_stream_out_device(HAL_AUDIO_DEVICE_HANDS_FREE_MONO);
    hal_audio_set_stream_out_volume(0xFFFFFA88, 0x0);

    log_hal_info("[MP3 Codec ut] play\n");
    hdl->play(hdl);
    vTaskDelay(20000 / portTICK_RATE_MS);

    log_hal_info("[MP3 Codec ut] pause\n");
    hdl->pause(hdl);
    vTaskDelay(5000 / portTICK_RATE_MS);

    log_hal_info("[MP3 Codec ut] resume\n");
    hdl->resume(hdl);
    vTaskDelay(5000 / portTICK_RATE_MS);

    log_hal_info("[MP3 Codec ut] pause\n");
    hdl->pause(hdl);
    vTaskDelay(5000 / portTICK_RATE_MS);

    log_hal_info("[MP3 Codec ut] resume\n");
    hdl->resume(hdl);
    vTaskDelay(5000 / portTICK_RATE_MS);

    while (1) {
        vTaskDelay(10 / portTICK_RATE_MS);
    }
}


