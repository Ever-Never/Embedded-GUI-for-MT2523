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

#ifndef __AMR_DECODER_H__
#define __AMR_DECODER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "hal_platform.h"


/**
 * @addtogroup Audio decoder
 * @{
 * @addtogroup Common
 * @{
 * @addtogroup AMR decoder
 * @{
 * This section introduces the AMR decoder APIs including terms and acronyms,
 * supported features, details on how to use the decoder, function groups, enums, structures and functions.
 *
 *
 *The Adaptive Multi-Rate (AMR or AMR-NB or GSM-AMR) audio codec is an audio compression format optimized for speech coding.
 *Sampling frequency is 8 kHz (160 samples for 20 ms frames). The AMR codec uses eight source codecs with bit-rates of 12.2, 10.2, 7.95, 7.40, 6.70, 5.90, 5.15 and 4.75 kbps.
 *
 * Enable AMR decoder function by adding the MTK_AUDIO_AMR_ENABLED definition in feature.mk file.
 * User must include audip_exp.h and define the IPCOMMON_PLUS for AMR decoder.
 *
 * @section AMR_DECODER_Terms_Chapter Terms and Acronyms
 *
 * |Terms               |Details                                                                                                                                                 |
 * |--------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |\b AMR              | Adaptive Multi-Rate. |
 * |\b NB                | Narrowband. |
 * |\b DTX              | Discontinuous Transmission . |
 * |\b CODEC          | A computer program for encoding or decoding a digital data stream or signal. |
 *
 *
 * @section amr_decoder_api_usage How to use this module
 *
 *
 * - The AMR decoder
 *   - Open and close the AMR decoder using #amr_decoder_open() and #amr_decoder_close().
 *   - The audio streaming data can be filled through a shared buffer.
 *    - Sample code:
 *     @code
 *
 *      #if defined(MTK_AUDIO_AMR_ENABLED)
 *      #include "amr_decoder.h"
 *      #include "audip_exp.h"
 *      IPCOMMON_PLUS //Must define it once for codec SWIP
 *      #endif
 *
 *       //This part will likely be different in your application.
 *
 *      static FATFS fatfs;
 *      static FIL fin;
 *      static FRESULT res;
 *      static UINT length_read;
 *
 *      static uint8_t share_buffer[2048];  //share buffer, user can allocate share buffer dynamically.
 *
 *      static void amr_decoder_event_callback(amr_decoder_handle_t *hdl, amr_decoder_media_event_t event)
 *      {
 *          uint8_t *share_buf;
 *          uint32_t share_buf_len;
 *
 *          switch (event) {
 *                  case AMR_DECODER_MEDIA_EVENT_ERROR:
 *                  // Error handling, such as notify the Application module or stop the AMR decoder.
 *                      break;
 *                  case AMR_DECODER_MEDIA_EVENT_REQUEST:
 *                  // Fill AMR streaming data to share buffer
 *                      uint32_t loop_idx;
 *                      uint32_t loop_cnt = 2;
 *                      for (loop_idx = 0; loop_idx < loop_cnt; loop_idx++) {
 *                              hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
 *                              f_read(&fin, share_buf, share_buf_len, &length_read);
 *                              log_hal_info("SD_EVENT:length_read from SD=%d", length_read);
 *                              hdl->write_data_done(hdl, length_read);
 *                      }
 *                      hdl->finish_write_data(hdl);
 *                      break;
 *          }
 *      }
 *
 *
 *      void amr_decoder_demo(void)
 *      {
 *          amr_decoder_handle_t *hdl = NULL;
 *          uint8_t *share_buf;
 *          uint32_t memory_size;
 *          uint32_t share_buf_len;
 *
 *          hdl = amr_decoder_open(amr_decoder_event_callback);
 *          if (hdl == NULL) {
 *              // Error handling
 *          }
 *
 *          if (open_file_from_sd(&fin, _T("SD:/AMR/in.amr")) < 0) {
 *              // Error handling
 *          }
 *
 *          //User can query memory size needed by decoder
 *          amr_decoder_get_memory_size(&memory_size);
 *          log_hal_info("memory_size needed by decoder=%d bytes", memory_size);
 *
 *          hdl->set_share_buffer(hdl, share_buffer, 2048);
 *
 *          //prefill data to share  buffer
 *          hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
 *          f_read(&fdst, share_buf, share_buf_len, &length_read);
 *          hdl->write_data_done(hdl, length_read);
 *          hdl->finish_write_data(hdl);
 *
 *          //play
 *          hdl->decoder_start(hdl);
 *
 *          //pause
 *          hdl->decoder_pause(hdl);
 *
 *          //resume
 *          hdl->decoder_resume(hdl);
 *
 *          //stop
 *          hdl->decoder_stop(hdl);
 *
 *          //close
 *          amr_decoder_close(hdl);
 *      }
 *
 *      @endcode
 */



/** @defgroup amr_decoder_enum Enum
  * @{
  */


/** @brief AMR decoder states. */
typedef enum {
    AMR_DECODER_STATE_IDLE = 0,         /**< The AMR decoder is inactive. */
    AMR_DECODER_STATE_READY,            /**< The AMR decoder is ready . */
    AMR_DECODER_STATE_DECODING,         /**< The AMR decoder is in a decoding state. */
    AMR_DECODER_STATE_PAUSE,            /**< The AMR decoder is in pause state. */
    AMR_DECODER_STATE_STOP,             /**< The AMR decoder has stopped. */
    AMR_DECODER_STATE_ERROR             /**< The AMR decoder reported an error. */
} amr_decoder_state_t;


/** @brief This structure defines the AMR decoder media events. */
typedef enum {
    AMR_DECODER_MEDIA_EVENT_ERROR ,                 /**< Failed, due to a bitstream error. */
    AMR_DECODER_MEDIA_EVENT_OK,                     /**< Successful. */
    AMR_DECODER_MEDIA_EVENT_UNDERFLOW,              /**< The bitstreaming is underflow and the AMR CODEC output is on silent. */
    AMR_DECODER_MEDIA_EVENT_REQUEST,                /**< Bitstreaming request from the user. */
    AMR_DECODER_MEDIA_EVENT_DATA_NOTIFICATION       /**< Bitstreaming notification to the user. */
} amr_decoder_media_event_t;


/** @brief This structure defines the AMR decoder return status. */
typedef enum {
    AMR_DECODER_STATUS_OK = 0,             /**< Successful. */
    AMR_DECODER_STATUS_ERROR = -1,         /**< Error. */
    AMR_DECODER_STATUS_INVALID_PARAM = -3  /**< Invalid parameters. */
} amr_decoder_status_t;


/**
 * @}
 */

/** @defgroup AMR_DECODER Struct
  * @{
  */

/** @brief This structure defines the share buffer structure. */
typedef struct {
    uint8_t         *buffer_base;       /**< Pointer to the ring buffer. */
    int32_t          buffer_size;       /**< Size of the ring buffer. (unit: Byte)*/
    int32_t          write;             /**< Index of the bitstream buffer to write the data. */
    int32_t          read;              /**< Index of the bitstream buffer to read the data. */
    void            *param;             /**< Extended parameter or information. */
} amr_decoder_share_buffer_t;


/** @brief AMR decoder handle structure type.*/
typedef struct _amr_decoder_handle_t {
    amr_decoder_state_t           state;               /**< The AMR decoder state. */
    amr_decoder_share_buffer_t    share_buffer;        /**< The share buffer for the AMR decoder. */
    bool                          underflow;           /**< The AMR decoder data underflow occurred. */
    bool                          waiting;             /**< The AMR decoder is waiting to fill data. */
    uint16_t                      audio_id;            /**< The audio ID of the AMR decoder. */

    void (*handler)(struct _amr_decoder_handle_t *handle, amr_decoder_media_event_t event_id);            /**< The AMR decoder handler. */
    void (*set_share_buffer)(struct _amr_decoder_handle_t *handle, uint8_t  *buffer, uint32_t  length);   /**< Set the shared buffer for bitstream. The audio streaming data can be filled through a shared buffer.*/
    void (*get_write_buffer)(struct _amr_decoder_handle_t *handle, uint8_t **buffer, uint32_t *length);   /**< Get the available length to write into a shared buffer and a pointer to the shared buffer. */
    void (*get_read_buffer)(struct _amr_decoder_handle_t *handle, uint8_t **buffer, uint32_t *length);    /**< Get the available length to read from a shared buffer and a pointer to the shared buffer. */
    void (*write_data_done)(struct _amr_decoder_handle_t *handle, uint32_t length);                       /**< Update the write pointer to the shared buffer. */
    void (*finish_write_data)(struct _amr_decoder_handle_t *handle);                                      /**< The last data transfer. */
    void (*reset_share_buffer)(struct _amr_decoder_handle_t *handle);                                     /**< Reset the shared buffer's information. */
    void (*read_data_done)(struct _amr_decoder_handle_t *handle, uint32_t length);                        /**< Update the read pointer to the shared buffer. */
    int32_t (*get_free_space)(struct _amr_decoder_handle_t *handle);                                      /**< Get the free space length available in the shared buffer. (in bytes)*/
    int32_t (*get_data_count)(struct _amr_decoder_handle_t *handle);                                      /**< Get the available data amount of the shared buffer. */

    amr_decoder_status_t (*decoder_start)(struct _amr_decoder_handle_t *handle);                          /**< Start the decoder. */
    amr_decoder_status_t (*decoder_stop)(struct _amr_decoder_handle_t *handle);                           /**< Stop the decoder. */
    amr_decoder_status_t (*decoder_pause)(struct _amr_decoder_handle_t *handle);                          /**< Pause the decoder. */
    amr_decoder_status_t (*decoder_resume)(struct _amr_decoder_handle_t *handle);                         /**< Resume the decoder. */
} amr_decoder_handle_t;



/** @brief  This defines the AMR decoder callback function prototype.
 *          The user should register a callback function while opening the AMR decoder. Please refer to #amr_decoder_open().
 *  @param[in] handle is the media handle of the AMR decoder.
 *  @param[in] event_id is the value defined in #amr_decoder_media_event_t. This parameter is given by the driver to notify the user about the data flow behavior.
 */
typedef void (*amr_decoder_callback_t)(amr_decoder_handle_t *handle, amr_decoder_media_event_t event_id);


/**
  * @}
  */

/**
 * @brief     This function opens the AMR decoder.
 * @param[in] callback is a pointer to the callback function for the AMR data control.
 * @return    Media handle of the AMR decoder. Please refer to #amr_decoder_handle_t.
 * @sa        #amr_decoder_close()
 */
amr_decoder_handle_t *amr_decoder_open(amr_decoder_callback_t callback);


/**
 * @brief     This function closes the AMR decoder.
 * @param[in] handle is the AMR decoder handle.
 * @return    If the operation completed successfully, the return value is #AMR_DECODER_STATUS_OK, otherwise the return value is #AMR_DECODER_STATUS_ERROR.
 * @sa        #amr_decoder_open()
 */
amr_decoder_status_t amr_decoder_close(amr_decoder_handle_t *handle);


/**
 * @brief     This function gets the memory requirment of the AMR decoder.
 * @param[out] memory_size is the amount of memory required for the AMR decoder for an internal use. (in bytes)
 * @return    If the operation completed successfully, the return value is #AMR_DECODER_STATUS_OK, otherwise the return value is #AMR_DECODER_STATUS_ERROR.
 */
amr_decoder_status_t amr_decoder_get_memory_size(uint32_t *memory_size);


#ifdef __cplusplus
}
#endif

/**
* @}
* @}
* @}
*/

#endif /* __AMR_DECODER_H__ */


