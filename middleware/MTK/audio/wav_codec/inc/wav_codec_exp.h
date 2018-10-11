/* Copyright Statement:
 *
 * (C) 2005-2017  MediaTek Inc. All rights reserved.
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

#ifndef __WAV_SWIP_LIB_
#define __WAV_SWIP_LIB_

//*******************************************************************************
//                         WAV_STATUS  
//*******************************************************************************

typedef enum { 
   WAV_FAIL        = 0,
   WAV_SUCCESS     = 1, 
   WAV_REFILL_INBUF= 2
} WAV_STATUS;

//*******************************************************************************
//                         WAV_FORMAT  
//*******************************************************************************

typedef enum { 
   WAV_FORMAT_LPCM     = 1,
   WAV_FORMAT_MSADPCM  = 2, 
   WAV_FORMAT_ALAW     = 6,
   WAV_FORMAT_ULAW     = 7, 
   WAV_FORMAT_DVIADPCM = 17,
   WAV_FORMAT_DRA      = 0xFFFE
} WAV_FORMAT;

//*******************************************************************************
//                         WAV_CONTAINER
//*******************************************************************************

typedef enum { 
   WAV_CONTAINER_WAV = 1,
   WAV_CONTAINER_AU, 
   WAV_CONTAINER_AIFF,
   WAV_CONTAINER_AIFC
} WAV_CONTAINER;

//*******************************************************************************
// STRUCTURE
//  WAV_DEC_HDL
//
// DESCRIPTION
//  WAV Codec handle
//
// MEMBERS
//  wav_dec_data       wav decoder internal buffer
//  format             coding format
//  container          container
//  data_offset        the offset (bytes) from file head to the first audio data
//  data_chunk_length  the total audio data number (bytes) in the file
//  bit_rate           file bit rate
//  total_duration     total duration (msec)
//  sample_rate        number of samples per second             
//  channel_num        channel number
//
//*******************************************************************************

typedef struct {
   void             *wav_dec_data;
   WAV_FORMAT       format;
   WAV_CONTAINER    container;
   unsigned int     data_offset;
   unsigned int     data_chunk_length;
   unsigned int     bit_rate;
   unsigned int     total_duration;
   unsigned short   sample_rate;
   unsigned char    channel_num;
}WAV_DEC_HDL;

//*******************************************************************************
// STRUCTURE
//  PCM_PARAM
//
// DESCRIPTION
//  PCM parameters for non-wav header file playback
//
// MEMBERS
//  format             coding format
//  DataLength         the total audio data number (bytes) in the file
//  SampleRate         number of samples per second             
//  SamplesPerBlock    Sample number in a block, for DVI-ADPCM format
//  BlockAlign         Block Alighed, for DVI-ADPCM format
//  BitsPerSample      number of bit per sample
//  ChannelNum         channel number
//*******************************************************************************

typedef struct {
   WAV_FORMAT       FormatTag;
   unsigned int     DataLength;
   unsigned short   SampleRate;
   unsigned short   SamplesPerBlock;
   unsigned short   BlockAlign;
   unsigned char    BitsPerSample;
   unsigned char    ChannelNum;
}PCM_PARAM;

//*******************************************************************************
// STRUCTURE
//  WAV_ENC_HDL
//
// DESCRIPTION
//  WAV Codec handle
//
// MEMBERS
//  wav_enc_data   wav encoder internal buffer
//  header_length  header length, the output file should reserved this offset for 
//                 file header. In other words, the audio data should begins at 
//                 file start + header_length
//*******************************************************************************

typedef struct {
   void             *wav_enc_data;
   unsigned int     header_length;
}WAV_ENC_HDL;

//*******************************************************************************
//                         DECLARATION OF PROTOTYPES
//*******************************************************************************

//*******************************************************************************
//  FUNCTION
//  WAV_GetDecBufferSize
//
//  DESCRIPTION
//  [DECODER] This function gets the buffers size for decoder.
//
//  PARAMETERS
//  bs_buf_size   [i] input bitstream buffer size (byte). 
//                    It should be a multiple number of 4. minimum size is 256
//  pcm_buf_size  [o] bitstream buffer size (byte). the size depends on the bs_buf_size  
//  int_buf_size  [o] internal working buffer size (byte)
//
//  RETURNS
//  WAV_SUCCESS, if the bs_buf_size is legal. 
//  If the bs_buf_size is illegal, WAV_FAIL is returned;
//
//*******************************************************************************
WAV_STATUS WAV_GetDecBufferSize(unsigned int bs_buf_size, 
                                unsigned int *pcm_buf_size, 
                                unsigned int *int_buf_size);

//*******************************************************************************
//  FUNCTION
//  WAV_InitDecoder
//
//  DESCRIPTION
//  [DECODER] Decoder initialization.
//
//  PARAMETERS
//  internal_buf  [i] internal working buffer. the size should as least equal to int_buf_size
//
//  RETURNS
//  decoder handle; WAV_BAD_FORMAT if initilation failed. 
//
//*******************************************************************************

WAV_DEC_HDL *WAV_InitDecoder(void *internal_buf);

//*******************************************************************************
//  FUNCTION
//  [DECODER] WAV_ParseHeader
//
//  DESCRIPTION
//  Parse header
//
//  PARAMETERS
//  hdl           [i] wav decoder handle
//  inbuf         [i] input buffer
//  inputBytes    [i] data counts(bytes) in input buffer
//  consumedBytes [o] the data counts(bytes) consumed during WAV_ParseHeader()
//
//  RETURNS
//  If the file is broken or not supported, WAV_FAIL is returned. 
//  If the header is parsed successfully, WAV_SUCCESS is returned. 
//  If the bitstream in the buffer is insufficient and need to be refilled, WAV_REFILL_INBUF is returned. 
//
//*******************************************************************************

WAV_STATUS WAV_ParseHeader(WAV_DEC_HDL  *hdl, 
                           const void   *inbuf, 
                           unsigned int inputBytes, 
                           unsigned int *consumedBytes);

//*******************************************************************************
//  FUNCTION
//  [DECODER] WAV_FillPCMParam
//
//  DESCRIPTION
//  Non-wav header PCM file playback, the bitstream information should provided by application
//
//  PARAMETERS
//  hdl           [i] wav handle
//  pcm_param     [1] bitstream information
//
//  RETURNS
//  WAV_SUCCESS is success, else returned WAV_FAIL;
//
//*******************************************************************************

WAV_STATUS WAV_FillPCMParam(WAV_DEC_HDL *hdl, PCM_PARAM *pcm_param); 

//*******************************************************************************
//  FUNCTION
//  [DECODER] WAV_Seek
//
//  DESCRIPTION
//  Seek the data offset of a sample
//
//  PARAMETERS
//  hdl           [i]   wav decoder handle
//  time          [i/o] input the start time (mini-second)
//                      output the exact start time that is sought
//
//  RETURNS
//  Offset (bytes) of the desired sample from the first audio data. (In other words, the 
//  real file offset is (returned value + header length in wav decoder handle) 
//   
//  If the given start time is larger than the total duration, 0xFFFFFFFF is returned. 
//
//*******************************************************************************

unsigned int WAV_Seek(WAV_DEC_HDL *hdl, unsigned int *time);

//*******************************************************************************
//  FUNCTION
//  [DECODER] WAV_Decode
//
//  DESCRIPTION
//  decoder function, decode bistream in input buffer and put the PCM to output buffer
//
//  PARAMETERS
//  hdl           [i]  wav decoder handle
//  inbuf         [i]  input buffer
//  inputBytes    [i]  input data(bytes) counts in input buffer
//  outbuf        [o]  output buffer
//  outputSamples [o]  output samples counts(words)
//
//  RETURNS
//  data counts(bytes) that is consumed by WAV_Decode()
//
//*******************************************************************************

unsigned int WAV_Decode(WAV_DEC_HDL  *hdl, 
                        const void   *inbuf , 
                        unsigned int inputBytes, 
                        void         *outbuf, 
                        unsigned int *outputSamples);

//*******************************************************************************
//  FUNCTION
//  [ENCODER] WAV_GetEncBufferSize
//
//  DESCRIPTION
//  This function gets the buffers size for encoder.
//
//  PARAMETERS
//  pcm_buf_size  [i] input bitstream buffer size (byte). 
//                    It should be a multiple number of 4. minimum size is 256
//  bs_buf_size   [o] bitstream buffer size (byte). the size depends on the bs_buf_size  
//  int_buf_size  [o] internal working buffer size (byte)
//
//  RETURNS
//  WAV_SUCCESS, if the bs_buf_size is legal. 
//  If the bs_buf_size is illegal, WAV_FAIL is returned;
//
//*******************************************************************************

WAV_STATUS WAV_GetEncBufferSize(unsigned int pcm_buf_size, 
                                unsigned int *bs_buf_size, 
                                unsigned int *int_buf_size);

//*******************************************************************************
//  FUNCTION
//  [ENCODER] WAV_InitEncoder
//
//  DESCRIPTION
//  Encoder initialization.
//
//  PARAMETERS
//  int_buffer   [i]   Internal buffer
//  Format       [i]   file format
//  SampleRate   [i]   file sample rate
//  ChannelNum   [i]   file channel number, only 1 and 2 are allowed.
//
//  RETURNS
//  WAV Encoder Handle; 0 if the input parameter is unsupported.
//
//*******************************************************************************

WAV_ENC_HDL *WAV_InitEncoder(void           *internal_buf, 
                             unsigned short Format, 
                             unsigned int   SampleRate, 
                             unsigned int   ChannelNum);

//*******************************************************************************
//  FUNCTION
//  [ENCODER] WAV_Encode
//
//  DESCRIPTION
//  encoder function, encode PCM data in input buffer and put bitstream to output buffer
//
//  PARAMETERS
//  hdl          [i] wav handle
//  inbuf        [i] input buffer
//  inputSamples [i] input samples counts(word)
//  outbuf       [o] output buffer
//  outputBytes  [o] output data counts(bytes)
//
//  RETURNS
//  samples count(word) that is consumed by WAV_Encode()
//
//*******************************************************************************

unsigned int WAV_Encode(WAV_ENC_HDL  *hdl, 
                        const void   *inbuf, 
                        unsigned int inputSamples, 
                        void         *outbuf, 
                        unsigned int *outputBytes);

//*******************************************************************************
//  FUNCTION
//  [ENCODER] WAV_EncHeader
//
//  DESCRIPTION
//  generate header to output buffer, the size equals to header_length in WAV_ENC_HDL
//
//  PARAMETERS
//  hdl           [i] wav handle
//  outbuf        [o] output buffer
//  data_length   [i] data length (bytes) that is encoded.
//
//  RETURNS
//  WAV_SUCCESS is success, else returned WAV_FAIL;
//
//*******************************************************************************

WAV_STATUS WAV_EncHeader(WAV_ENC_HDL  *hdl, 
                         void         *outbuf,
                         unsigned int data_length); 
#endif



