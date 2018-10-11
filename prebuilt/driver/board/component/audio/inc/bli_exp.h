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

#ifndef BLI_EXP_H
#define BLI_EXP_H

typedef void     BLI_HANDLE;

#ifndef NULL
#define NULL    0
#endif


/*----------------------------------------------------------------------*/
/* Get required buffer size for BLI Software SRC                        */
/*----------------------------------------------------------------------*/
void BLI_GetMemSize(unsigned int inSR,                  /* Input, input sampling rate of the conversion */
                    unsigned int inChannel,             /* Input, input channel number of the conversion */
                    unsigned int outSR,                 /* Input, output sampling rate of the conversion */
                    unsigned int outChannel,            /* Input, output channel number of the conversion */
                    unsigned int *workBufSize);         /* Output, the required working buffer size in byte */

/*----------------------------------------------------------------------*/
/* Get the BLI Software SRC handler.                                    */
/* Return: the handle of current BLI Software SRC                       */
/*----------------------------------------------------------------------*/
BLI_HANDLE *BLI_Open(unsigned int inSR,                 /* Input, input sampling rate of the conversion */
                     unsigned int inChannel,            /* Input, input channel number of the conversion */
                     unsigned int outSR,                /* Input, output sampling rate of the conversion */
                     unsigned int outChannel,           /* Input, output channel number of the conversion */
                     char* buffer,                      /* Input, pointer to the working buffer */
                     void *(*custom_alloc)(unsigned int));

/*----------------------------------------------------------------------*/
/* Decompress the bitstream to PCM data                                 */
/* Return: consumed input buffer size(byte)                             */
/*----------------------------------------------------------------------*/
unsigned int BLI_Convert(void *hdl,                  /* Input, handle of this conversion */
                         short *inBuf,               /* Input, pointer to input buffer */
                         unsigned int *inLength,     /* Input, length(byte) of input buffer */
                                                     /* Output, length(byte) left in the input buffer after conversion */
                         short *outBuf,              /* Input, pointer to output buffer */
                         unsigned int *outLength);   /* Input, length(byte) of output buffer */
                                                     /* Output, output data length(byte) */

/*----------------------------------------------------------------------*/
/* Close the process                                                    */
/*----------------------------------------------------------------------*/
void BLI_Close( void *hdl,
                void (*custom_free) (void *));

/*----------------------------------------------------------------------*/
/* Clear the internal status for the discontinuous input buffer         */
/*----------------------------------------------------------------------*/
int BLI_Reset(void *hdl);

#endif

