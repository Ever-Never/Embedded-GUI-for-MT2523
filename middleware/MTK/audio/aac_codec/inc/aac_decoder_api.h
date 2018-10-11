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

#ifndef __AAC_DECODER_API_H__
#define __AAC_DECODER_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "hal_platform.h"


/**
 * @addtogroup Audio Decoder
 * @{
 * @addtogroup Common
 * @{
 * @addtogroup AAC decoder
 * @{
 * This section introduces the AAC decoder APIs including terms and acronyms,
 * supported features, details on how to use the decoder, function groups, enums, structures and functions.
 *
 * The AAC decoder is used for AAC-LC only or both HE-AAC V1 and AAC-LC formats depending on the compiler option.
 * MTK_AUDIO_AAC_DECODER_ENABLED for AAC-LC only, MTK_AUDIO_HEAAC_DECODER_ENABLED for HE-AAC V1 and AAC-LC. User can choose either one of them.
 *
 * @section AAC_DECODER_Terms_Chapter Terms and Acronyms
 *
 * |Terms               |Details                                                                                                                                                 |
 * |--------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |\b A2DP            | The Advanced Audio Distribution Profile (A2DP) defines the protocols and procedures that implement distribution of audio content of high-quality in mono or stereo on ACL channels. |
 * |\b AAC              | Advanced Audio Coding (AAC) is an audio coding standard for lossy digital audio compression. |
 * |\b HE-AAC V1   | High Efficiency AAC version 1 profile (HE-AAC v1) uses spectral band replication (SBR) to enhance the compression efficiency in the frequency domain. |
 * |\b SBC              | The Low Complexity Subband Coding (SBC) is an audio subband codec specified by the Bluetooth Special Interest Group (SIG) for the Advanced Audio Distribution Profile (A2DP). SBC is a digital audio encoder and decoder used to transfer data to Bluetooth audio output devices like headphones or loudspeakers. |
 * |\b VBR              | Variable bitrate (VBR) is a term used in telecommunications and computing that relates to the bitrate used in sound or video encoding. |
 *
 */


/**
 * @defgroup aac_decoder_media_events_define Define
 * @{
 */

/** @brief Defines the AAC decoder media event. */
#define AAC_DECODER_API_MEDIA_EVENT_START    (100)      /**< The starting base of the media event. */

/** @brief Defines the minimum required size of bitstream buffer. */
#define SHARE_BUFF_SIZE                      3192       /**< The minimum required size of bitstream buffer (unit: byte). */

/** @brief AAC decoder states. */
typedef enum {
    AAC_DECODER_API_STATE_IDLE,         /**< The AAC decoder is inactive. */
    AAC_DECODER_API_STATE_READY,        /**< The AAC decoder is ready to play the media. */
    AAC_DECODER_API_STATE_PLAY,         /**< The AAC decoder is in a playing state. */
    AAC_DECODER_API_STATE_PAUSE,        /**< The AAC decoder is in pause state. */
    AAC_DECODER_API_STATE_RESUME,       /**< The AAC decoder is in resume state. */
    AAC_DECODER_API_STATE_STOP,         /**< The AAC decoder has stopped. */
    AAC_DECODER_API_STATE_ERROR         /**< The AAC decoder reported an error. */
} aac_decoder_api_state_t;


/** @brief This structure defines the AAC decoder media events. */
typedef enum {
    AAC_DECODER_API_MEDIA_EVENT_ERROR = AAC_DECODER_API_MEDIA_EVENT_START,  /**< Failed, due to a bitstream error. */
    AAC_DECODER_API_MEDIA_EVENT_OK,                                         /**< Successful. */
    AAC_DECODER_API_MEDIA_EVENT_UNDERFLOW,                                  /**< The bitstreaming is underflow and the AAC decoder output is on silent. */
    AAC_DECODER_API_MEDIA_EVENT_REQUEST,                                    /**< Bitstreaming request from the user. */
    AAC_DECODER_API_MEDIA_EVENT_DATA_NOTIFICATION                           /**< Bitstreaming notification to the user. */
} aac_decoder_api_media_event_t;


/** @brief This structure defines the AAC decoder API return status. */
typedef enum {
    AAC_DECODER_API_STATUS_OK = 0,             /**< Successful. */
    AAC_DECODER_API_STATUS_ERROR = -1,         /**< Error. */
    AAC_DECODER_API_STATUS_INVALID_PARAM = -3  /**< Invalid parameters. */
} aac_decoder_api_status_t;

/**
 * @}
 */

/** @defgroup aac_decoder_struct Struct
  * @{
  */

/** @brief This structure defines the shared buffer structure. */
typedef struct {
    uint8_t         *buffer_base;       /**< Pointer to the ring buffer. */
    int32_t          buffer_size;       /**< Size of the ring buffer. (unit: Byte)*/
    int32_t          write;             /**< Index of the bitstream buffer to write the data. */
    int32_t          read;              /**< Index of the bitstream buffer to read the data. */
    void            *param;             /**< Extended parameter or information. */
} aac_decoder_api_share_buffer_t;


/** @brief HE-AAC audio handle structure type.*/
typedef struct _aac_decoder_api_media_handle_t {
    aac_decoder_api_state_t           state;                /**< The AAC decoder state. */
    aac_decoder_api_share_buffer_t    share_buffer;         /**< The shared buffer for the AAC decoder. */
    bool                              underflow;            /**< The AAC decoder data underflow occurred. */
    bool                              waiting;              /**< The AAC decoder is waiting to fill data. */
    uint16_t                          audio_id;             /**< The audio ID of the AAC decoder. */

    void (*handler)(struct _aac_decoder_api_media_handle_t *handle, aac_decoder_api_media_event_t event_id);                /**< The AAC decoder handler. */
    void (*set_share_buffer)(struct _aac_decoder_api_media_handle_t *handle, uint8_t  *buffer, uint32_t  length);           /**< Set the shared buffer for bitstream. The audio streaming data can be filled through a shared buffer.*/
    void (*get_write_buffer)(struct _aac_decoder_api_media_handle_t *handle, uint8_t **buffer, uint32_t *length);           /**< Get the available length to write into a shared buffer and a pointer to the shared buffer. */
    void (*get_read_buffer)(struct _aac_decoder_api_media_handle_t *handle, uint8_t **buffer, uint32_t *length);            /**< Get the available length to read from a shared buffer and a pointer to the shared buffer. */
    void (*write_data_done)(struct _aac_decoder_api_media_handle_t *handle, uint32_t length);                               /**< Update the write pointer to the shared buffer. */
    void (*finish_write_data)(struct _aac_decoder_api_media_handle_t *handle);                                              /**< The last data transfer. */
    void (*reset_share_buffer)(struct _aac_decoder_api_media_handle_t *handle);                                             /**< Reset the shared buffer's information. */
    void (*read_data_done)(struct _aac_decoder_api_media_handle_t *handle, uint32_t length);                                /**< Update the read pointer to the shared buffer. */
    int32_t (*get_free_space)(struct _aac_decoder_api_media_handle_t *handle);                                              /**< Get the free space length available in the shared buffer. (in bytes)*/
    int32_t (*get_data_count)(struct _aac_decoder_api_media_handle_t *handle);                                              /**< Get the available data amount of the shared buffer. */

    aac_decoder_api_status_t (*play)(struct _aac_decoder_api_media_handle_t *handle);                                           /**< The AAC decoder play function. */
    aac_decoder_api_status_t (*pause)(struct _aac_decoder_api_media_handle_t *handle);                                          /**< The AAC decoder pause function. */
    aac_decoder_api_status_t (*resume)(struct _aac_decoder_api_media_handle_t *handle);                                         /**< The AAC decoder resume function. */
    aac_decoder_api_status_t (*process)(struct _aac_decoder_api_media_handle_t *handle, aac_decoder_api_media_event_t event_id);  /**< The AAC decoder process function. */
    aac_decoder_api_status_t (*stop)(struct _aac_decoder_api_media_handle_t *handle);                                           /**< The AAC decoder stop function. */
    aac_decoder_api_status_t (*flush)(struct _aac_decoder_api_media_handle_t *handle,int32_t flush_data_flag); 
} aac_decoder_api_media_handle_t;

/**
 *  @brief This structure defines the AAC decoder runtime information.
 */
typedef struct {
    uint32_t sampling_rate;     /**< Sampling rate of the output PCM data. (unit: Hz) */
    uint32_t channel_number;    /**< Channel number of the output PCM data.           */
    uint32_t frame_size;        /**< PCM frame size per channel. (unit: sample)       */
    uint32_t sbr_flag;          /* SBR bitstream detected or not
                                                        0 --> no SBR
                                                        1 --> with SBR                                  */
} aac_decoder_api_runtime_information_t;


/** @brief  This defines the AAC decoder callback function prototype.
 *          The user should register a callback function while opening the AAC decoder. Please refer to #aac_deocder_api_open().
 *  @param[in] handle is the media handle of the AAC decoder.
 *  @param[in] event_id is the value defined in #aac_decoder_api_media_event_t. This parameter is given by the driver to notify the user about the data flow behavior.
 */
typedef void (*aac_decoder_api_callback_t)(aac_decoder_api_media_handle_t *handle, aac_decoder_api_media_event_t event_id);


/**
  * @}
  */

/**
 * @brief     This function opens the AAC decoder.
 * @param[in] callback is a pointer to the callback function for the AAC data control.
 * @return    Media handle of the A2DP. Please refer to #aac_decoder_api_media_handle_t.
 * @sa        #aac_decoder_api_close()
 */
aac_decoder_api_media_handle_t *aac_deocder_api_open(aac_decoder_api_callback_t callback);


/**
 * @brief     This function closes the AAC decoder.
 * @param[in] handle is the AAC decoder handle.
 * @return    If the operation completed successfully, the return value is #AAC_DECODER_API_STATUS_OK, otherwise the return value is #AAC_DECODER_API_STATUS_ERROR.
 * @sa        #aac_deocder_api_open()
 */
aac_decoder_api_status_t aac_decoder_api_close(aac_decoder_api_media_handle_t *handle);


/**
 * @brief     This function gets the version of the decoder library.
 * @return    The return value is version number.
 */
uint32_t aac_decoder_api_get_version(void);

#ifdef __cplusplus
}
#endif

/**
* @}
* @}
* @}
*/

#endif /* __AAC_DECODER_API_H__ */

