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

#ifndef __AMR_ENCODER_H__
#define __AMR_ENCODER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "hal_platform.h"


/**
 * @addtogroup Audio encoder
 * @{
 * @addtogroup Common
 * @{
 * @addtogroup AMR encoder
 * @{
 * This section introduces the AMR encoder APIs including terms and acronyms,
 * supported features, details on how to use the encoder, function groups, enums, structures and functions.
 *
 *
 *The Adaptive Multi-Rate (AMR or AMR-NB or GSM-AMR) audio codec is an audio compression format optimized for speech coding.
 *Sampling frequency is 8 kHz (160 samples for 20 ms frames). The AMR codec uses eight source codecs with bit-rates of 12.2, 10.2, 7.95, 7.40, 6.70, 5.90, 5.15 and 4.75 kbps.
 *
 * Enable AMR decoder function by adding the MTK_AUDIO_AMR_ENABLED definition in feature.mk file.
 * User must include audip_exp.h and define the IPCOMMON_PLUS for AMR encoder.
 *
 * @section AMR_CODEC_Terms_Chapter Terms and Acronyms
 *
 * |Terms               |Details                                                                                                                                                 |
 * |--------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |\b AMR              | Adaptive Multi-Rate. |
 * |\b NB                | Narrowband. |
 * |\b DTX              | Discontinuous Transmission . |
 * |\b CODEC         | A computer program for encoding or decoding a digital data stream or signal. |
 *
 *
 * @section amr_encoder_api_usage How to use this module
 *
 * - The AMR encoder
 *   - Open and close the AMR encoder using #amr_encoder_open() and #amr_encoder_close().
 *   - The audio streaming data can be filled through a shared buffer.
 *    - Sample code:
 *     @code
 *
 *      #if defined(MTK_AUDIO_AMR_ENABLED)
 *      #include "amr_encoder.h"
 *      #include "audip_exp.h"
 *      IPCOMMON_PLUS //Must define it once for codec SWIP
 *      #endif
 *
 *       //This demo will likely be different in your application.
 *
 *       static FATFS fatfs;
 *       static FIL fin;
 *       static FIL fout;
 *       static FRESULT res;
 *       static UINT length_read, length_written;
 *
 *      static uint8_t share_buffer[2048];  //share buffer, user can allocate share buffer dynamically.
 *
 *      static void amr_encoder_event_callback(amr_encoder_handle_t *hdl, amr_decoder_media_event_t event)
 *      {
 *          uint8_t *share_buf;
 *          uint32_t share_buf_len;
 *
 *          switch (event) {
 *                  case AMR_ENCODER_MEDIA_EVENT_ERROR:
 *                  // Error handling, such as notify the Application module or stop the AMR encoder.
 *                      break;
 *                  case AMR_ENCODER_MEDIA_EVENT_REQUEST:
 *                  // Fill AMR streaming data to share buffer
 *                      uint32_t loop_idx;
 *                      uint32_t loop_cnt = 2;
 *                      for (loop_idx = 0; loop_idx < loop_cnt; loop_idx++) {
 *                              hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
 *                              f_read(&fdst, share_buf, share_buf_len, &length_read);
 *                              log_hal_info("SD_EVENT:length_read from SD=%d", length_read);
 *                              hdl->write_data_done(hdl, length_read);
 *                      }
 *                      hdl->finish_write_data(hdl);
 *                      break;
 *                 case AMR_ENCODER_MEDIA_EVENT_DATA_NOTIFICATION:
 *                  // Get encoded data from encoder
 *                      uint8_t *base;
 *                      uint32_t len;
 *                      hdl->get_encoded_data_info(&base,&len);
 *                      f_write(&fout, base, len, &length_written);
 *                      hdl->get_encoded_data_done(length_written);
 *                      break;
 *          }
 *      }
 *
 *
 *      void amr_encoder_demo(void)
 *      {
 *          amr_encoder_handle_t *hdl = NULL;
 *          amr_encoder_config_t encoder_config;
 *          uint8_t *share_buf;
 *          uint32_t memory_size;
 *          uint32_t share_buf_len;
 *
 *          encoder_config.bitrate=AMR_ENCODER_12_20KBPS;
 *          encoder_config.dtx_mode=AMR_ENCODER_DTX_DISABLE;
 *
 *          hdl = amr_encoder_open(amr_decoder_event_callback,encoder_config);
 *          if (hdl == NULL) {
 *              // Error handling
 *          }
 *
 *          if (open_file_from_sd(&fin, _T("SD:/AMR/in.pcm")) < 0) {
 *              // Error handling
 *          }
 *
 *          //User can query memory size needed by encoder
 *          amr_encoder_get_memory_size(&memory_size);
 *
 *          hdl->set_share_buffer(hdl, share_buffer, 2048);
 *
 *          //prefill data to share  buffer
 *          hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
 *          f_read(&fin, share_buf, share_buf_len, &length_read);
 *          hdl->write_data_done(hdl, length_read);
 *          hdl->finish_write_data(hdl);
 *
 *          //Create a new file for writing
 *          res = f_open(&fout, _T("SD:/AMR/in.amr"), FA_CREATE_ALWAYS);
 *          if (FR_OK != res) {
 *               // Error handling
 *          }
 *
 *          res = f_open(&fout, _T("SD:/AMR/in.amr"), FA_OPEN_EXISTING | FA_WRITE);
 *          if (FR_OK != res) {
 *            // Error handling
 *          }
 *
 *          //Write magic number to indicate single channel AMR file storage format
 *          char *str="#!AMR\n";
 *          f_write(&fout, str, 6, &length_written);
 *
 *          //encode
 *          hdl->encoder_start(hdl);
 *
 *          //stop
 *          hdl->encoder_stop(hdl);
 *
 *          //close
 *          amr_encoder_close(hdl);
 *
 *          f_close(&fin);
 *          f_close(&fout);
 *
 *      }
 *
 *      @endcode
 */


/** @defgroup ame_encoder_enum Enum
  * @{
  */


/** @brief AMR encoder states. */
typedef enum {
    AMR_ENCODER_STATE_IDLE = 0,         /**< The AMR encoder is inactive. */
    AMR_ENCODER_STATE_READY,            /**< The AMR encoder is ready to encode. */
    AMR_ENCODER_STATE_ENCODING,         /**< The AMR encoder is in an encoding state. */
    AMR_ENCODER_STATE_STOP,             /**< The AMR encoder has stopped. */
    AMR_ENCODER_STATE_ERROR             /**< The AMR encoder reported an error. */
} amr_encoder_state_t;


/** @brief This structure defines the AMR encoder media events. */
typedef enum {
    AMR_ENCODER_MEDIA_EVENT_ERROR ,             /**< Failed, due to a bitstream error. */
    AMR_ENCODER_MEDIA_EVENT_OK,                 /**< Successful. */
    AMR_ENCODER_MEDIA_EVENT_UNDERFLOW,          /**< The bitstreaming is underflow  */
    AMR_ENCODER_MEDIA_EVENT_REQUEST,            /**< Bitstreaming request from the user. */
    AMR_ENCODER_MEDIA_EVENT_DATA_NOTIFICATION,  /**< Bitstreaming notification to the user. */
    AMR_ENCODER_MEDIA_EVENT_FLUSH_DONE          /**< Flush completion notification to the user. */
} amr_encoder_media_event_t;


/** @brief This structure defines the AMR encoder return status. */
typedef enum {
    AMR_ENCODER_STATUS_OK = 0,             /**< Successful. */
    AMR_ENCODER_STATUS_ERROR = -1,         /**< Error. */
    AMR_ENCODER_STATUS_INVALID_PARAM = -3  /**< Invalid parameters. */
} amr_encoder_status_t;


/** @brief This structure defines the DTX support is enabled or not. */
typedef enum {
    AMR_ENCODER_DTX_DISABLE  = 0,     /**< Disable DTX support. */
    AMR_ENCODER_DTX_ENABLE   = 1      /**< Enable DTX support. */
} amr_encoder_dtx_mode_t;


/** @brief This structure defines the bit-rates. */
typedef enum {
    AMR_ENCODER_4_75KBPS   = 0,      /**< 4.75 kbit/s. */
    AMR_ENCODER_5_15KBPS   = 1,      /**< 5.15 kbit/s. */
    AMR_ENCODER_5_90KBPS   = 2,      /**< 5.90 kbit/s. */
    AMR_ENCODER_6_70KBPS   = 3,      /**< 6.70 kbit/s. */
    AMR_ENCODER_7_40KBPS   = 4,      /**< 7.40 kbit/s. */
    AMR_ENCODER_7_95KBPS   = 5,      /**< 7.95 kbit/s. */
    AMR_ENCODER_10_20KBPS  = 6,      /**< 10.20 kbit/s. */
    AMR_ENCODER_12_20KBPS  = 7,      /**< 12.20 kbit/s. */
    AMR_ENCODER_DTX        = 8       /**< DTX. */
} amr_encoder_bitrate_t;

/**
 * @}
 */

/** @defgroup AMR_ENCODER Struct
  * @{
  */

/** @brief This structure defines the share buffer structure. */
typedef struct {
    uint8_t         *buffer_base;       /**< Pointer to the ring buffer. */
    int32_t          buffer_size;       /**< Size of the ring buffer. (unit: Byte)*/
    int32_t          write;             /**< Index of the bitstream buffer to write the data. */
    int32_t          read;              /**< Index of the bitstream buffer to read the data. */
    void            *param;             /**< Extended parameter or information. */
} amr_encoder_share_buffer_t;



/**
 *  @brief This structure defines the AMR encoder configuration.
 */
typedef struct {
    amr_encoder_dtx_mode_t dtx_mode;      /**< DTX enable or not. */
    amr_encoder_bitrate_t bitrate;        /**< Encoder bit-rates. */
} amr_encoder_config_t;


/** @brief AMR ENCODER handle structure type.*/
typedef struct _amr_encoder_handle_t {
    amr_encoder_state_t         state;                /**< The AMR encoder state. */
    amr_encoder_share_buffer_t  share_buffer;         /**< The shared buffer for the AMR encoder. */
    bool                        underflow;            /**< The AMR encoder data underflow occurred. */
    bool                        write_waiting;              /**< The AMR encoder is waiting to fill data. */
    bool                        read_waiting;              /**< The AMR encoder is waiting user to get data. */
    uint16_t                    audio_id;             /**< The audio ID of the AMR encoder. */

    void (*handler)(struct _amr_encoder_handle_t *handle, amr_encoder_media_event_t event_id);              /**< The AMR encoder handler. */
    void (*set_share_buffer)(struct _amr_encoder_handle_t *handle, uint8_t  *buffer, uint32_t  length);     /**< Set the shared buffer for bitstream. The audio streaming data can be filled through a shared buffer.*/
    void (*get_write_buffer)(struct _amr_encoder_handle_t *handle, uint8_t **buffer, uint32_t *length);     /**< Get the available length to write into a shared buffer and a pointer to the shared buffer. */
    void (*get_read_buffer)(struct _amr_encoder_handle_t *handle, uint8_t **buffer, uint32_t *length);      /**< Get the available length to read from a shared buffer and a pointer to the shared buffer. */
    void (*write_data_done)(struct _amr_encoder_handle_t *handle, uint32_t length);                         /**< Update the write pointer to the shared buffer. */
    void (*finish_write_data)(struct _amr_encoder_handle_t *handle);                                        /**< The last data transfer. */
    void (*reset_share_buffer)(struct _amr_encoder_handle_t *handle);                                       /**< Reset the shared buffer's information. */
    void (*read_data_done)(struct _amr_encoder_handle_t *handle, uint32_t length);                          /**< Update the read pointer to the shared buffer. */
    int32_t (*get_free_space)(struct _amr_encoder_handle_t *handle);                                        /**< Get the free space length available in the shared buffer. (in bytes)*/
    int32_t (*get_data_count)(struct _amr_encoder_handle_t *handle);                                        /**< Get the available data amount of the shared buffer. */

    amr_encoder_status_t (*encoder_start)(struct _amr_encoder_handle_t *handle);                            /**< Start to encoder. */
    amr_encoder_status_t (*encoder_stop)(struct _amr_encoder_handle_t *handle);                             /**< Stop to encoder. */
    amr_encoder_status_t (*encoder_flush)(struct _amr_encoder_handle_t *handle,int32_t flush_data_flag);    /**< Flush data to encoder. */
    amr_encoder_status_t (*get_encoded_data_info)(uint8_t **base_pointer, uint32_t *length);                /**< Get encoded data info from encoder. */
    amr_encoder_status_t (*get_encoded_data_done)(uint32_t length);                                         /**< Notify encoder of getting encoded data done. */
} amr_encoder_handle_t;


/** @brief  This defines the AMR encoder callback function prototype.
 *          The user should register a callback function while opening the AMR encoder. Please refer to #amr_encoder_open().
 *  @param[in] handle is the media handle of the AMR encoder.
 *  @param[in] event_id is the value defined in #amr_encoder_media_event_t. This parameter is given by the driver to notify the user about the data flow behavior.
 */
typedef void (*amr_encoder_callback_t)(amr_encoder_handle_t *handle, amr_encoder_media_event_t event_id);


/**
  * @}
  */

/**
 * @brief     This function opens the AMR encoder.
 * @param[in] callback is a pointer to the callback function for the AMR data control.
 * @param[in] encoder_config is the settings of the AMR encoder..
 * @return    Media handle of the AMR encoder. Please refer to #amr_encoder_handle_t.
 * @sa        #amr_encoder_close()
 */
amr_encoder_handle_t *amr_encoder_open(amr_encoder_callback_t callback, const amr_encoder_config_t encoder_config);


/**
 * @brief     This function closes the AMR encoder.
 * @param[in] handle is the AMR encoder handle.
 * @return    If the operation completed successfully, the return value is #AMR_ENCODER_STATUS_OK, otherwise the return value is #AMR_ENCODER_STATUS_ERROR.
 * @sa        #amr_encoder_open()
 */
amr_encoder_status_t amr_encoder_close(amr_encoder_handle_t *handle);


/**
 * @brief     This function gets the memory requirment of the AMR encoder.
 * @param[out] memory_size is the amount of memory required for the AMR encoder for an internal use. (in bytes)
 * @return    If the operation completed successfully, the return value is #AMR_CODEC_STATUS_OK, otherwise the return value is #AMR_CODEC_STATUS_ERROR.
 */
amr_encoder_status_t amr_encoder_get_memory_size(uint32_t *memory_size);


#ifdef __cplusplus
}
#endif

/**
* @}
* @}
* @}
*/

#endif /* __AMR_ENCODER_H__ */


