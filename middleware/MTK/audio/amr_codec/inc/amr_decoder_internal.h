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



#ifndef __AMR_DECODER_INTERNAL_H__
#define __AMR_DECODER_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdbool.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "syslog.h"
#include "memory_attribute.h"
#include "task_def.h"
#include "hal_platform.h"
#include "hal_gpt.h"
/*for GPT time*/
#include "hal_gpt.h"
/*AMR_NB_CODEC SWIP*/
#include "amr_exp.h"
//#include "audip_exp.h"
/*AMR decoder driver*/
#include "amr_decoder.h"
#include "hal_audio_internal_service.h"


/** @brief Defines the AMR decoder queue information. */
#define     MAX_AMR_DECODER_FUNCTIONS          5      /** The maximum number of callback function. */
#define     AMR_DECODER_QUEUE_SIZE             10     /** The maximum number of items the queue can hold at any one time.*/


#define AMR_DECODER_LOG_ENABLE

log_create_module(AMR_DEC, PRINT_LEVEL_INFO);

#define LOGE(fmt,arg...)   LOG_E(AMR_DEC, "[AMR DEC]: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(AMR_DEC, "[AMR DEC]: "fmt,##arg)

#if defined AMR_DECODER_LOG_ENABLE
#define LOGI(fmt,arg...)   LOG_I(AMR_DEC, "[AMR DEC]: "fmt,##arg)
#else
#define LOGI(fmt,arg...)
#endif

typedef enum {
    AMR_DECODER_QUEUE_EVENT_NONE = 0,
    AMR_DECODER_QUEUE_EVENT_DECODE,
    AMR_DECODER_QUEUE_EVENT_FILL_DATA,
    AMR_DECODER_QUEUE_EVENT_TOTAL
} amr_decoder_queue_event_id_t;


typedef struct {
    amr_decoder_handle_t         handle;                      /** AMR decoder handle */
    ring_buffer_information_t    stream_out_pcm_buff;         /** This buffer contains the pcm data that are ready to play */

    /*decoder handler*/
    AMR_DEC_HANDLE               *amr_dec_handle;             /** AMR CODEC IP used.  */

    /*decoder buffer and buffer size*/
    void                        *dec_int_buffer;              /** AMR CODEC IP used.  */
    void                        *dec_tmp_buffer;              /** AMR CODEC IP used.  */
    int16_t                     *dec_pcm_buffer;              /** AMR CODEC IP used. Store decoded pcm data.*/
    uint8_t                     *dec_bs_buffer;               /** AMR CODEC IP used. Store encoded stream to decode*/

    uint32_t                    dec_int_buf_size;              /* The internal buffer size (1636 bytes)*/
    uint32_t                    dec_tmp_buf_size;              /* The temp buffer size (5488 bytes)*/
    uint32_t                    dec_pcm_buf_size;              /* The pcm buffer size (320 bytes)*/
    uint32_t                    dec_bs_buf_size;               /* The bs buffer size (32 bytes)*/

    uint32_t                    stream_out_pcm_buff_size;      /* stream out pcm buffer size */

} amr_decoder_internal_handle_t;


typedef struct {
    amr_decoder_queue_event_id_t id;
    void *parameter;
} amr_decoder_queue_event_t;


typedef void (*amr_decoder_internal_callback_t)(void *parameter);


#ifdef __cplusplus
}
#endif

#endif  /*__AMR_DECODER_INTERNAL_H__*/

