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


#ifndef MP3_WAV_H
#define MP3_WAV_H

#include <stdio.h>  /*for FILE pointer*/


/*********************
     STRUCTURE
*********************/

struct
{
   unsigned char riffheader[4];
   unsigned char WAVElen[4];
   struct
   {
      unsigned char fmtheader[8];
      unsigned char fmtlen[4];
      struct
      {
         unsigned char FormatTag[2];
         unsigned char Channels[2];
         unsigned char SamplesPerSec[4];
         unsigned char AvgBytesPerSec[4];
         unsigned char BlockAlign[2];
         unsigned char BitsPerSample[2]; /* format specific for PCM */
      } fmt;
      struct
      {
         unsigned char dataheader[4];
         unsigned char datalen[4];
         /* from here you insert your PCM data */
      } data;
   } WAVE;
} RIFF =
{  { 'R','I','F','F' } ,{ sizeof(RIFF.WAVE),0,0,0 } ,
   {  { 'W','A','V','E','f','m','t',' ' } , { sizeof(RIFF.WAVE.fmt),0,0,0} ,
      { {1,0} , {0,0},{0,0,0,0},{0,0,0,0},{0,0},{16,0} } ,
      { { 'd','a','t','a' }  , {0,0,0,0} }
   }
};

/*********************
     TABLE
*********************/
static int rates[9] = {
	44100, 48000, 32000,
	22050, 24000, 16000,
	11025, 12000,  8000
};


/*********************
     SUB_FUNCION
*********************/

static void long2littleendian(long inval,unsigned char *outval,int b)
{
  int i;
  for(i=0;i<b;i++) {
    outval[i] = (inval>>(i*8)) & 0xff;
  }
}

int wav_open(int rn, int c, FILE *wavfp)
{
   int bps = 16;

   long2littleendian(c,RIFF.WAVE.fmt.Channels,sizeof(RIFF.WAVE.fmt.Channels));
   long2littleendian(rates[rn],RIFF.WAVE.fmt.SamplesPerSec,sizeof(RIFF.WAVE.fmt.SamplesPerSec));
   long2littleendian((int)(c * rates[rn] * bps)>>3, RIFF.WAVE.fmt.AvgBytesPerSec,sizeof(RIFF.WAVE.fmt.AvgBytesPerSec));
   long2littleendian((int)(c * bps)>>3, RIFF.WAVE.fmt.BlockAlign,sizeof(RIFF.WAVE.fmt.BlockAlign));
   long2littleendian(0,RIFF.WAVE.data.datalen,sizeof(RIFF.WAVE.data.datalen));
   long2littleendian(0+sizeof(RIFF.WAVE),RIFF.WAVElen,sizeof(RIFF.WAVElen));
   fwrite(&RIFF, sizeof(RIFF),1,wavfp);

   return 0;
}

int wav_write(short *buf,int len, FILE *wavfp)
{
   int temp;

   if(!wavfp)
     return 0;

   temp = fwrite(buf, 1, len*2, wavfp);
   if(temp <= 0)
     return 0;

   return temp;
}

int wav_close(FILE *wavfp, int datalen)
{
   if(!wavfp)
      return 0;

   if(fseek(wavfp, 0L, SEEK_SET) >= 0) {
     long2littleendian(datalen,RIFF.WAVE.data.datalen,sizeof(RIFF.WAVE.data.datalen));
     long2littleendian(datalen+sizeof(RIFF.WAVE),RIFF.WAVElen,sizeof(RIFF.WAVElen));
     fwrite(&RIFF, sizeof(RIFF),1,wavfp);
   }
   else {
     fprintf(stderr,"Warning can't rewind WAV file. File-format isn't fully conform now.\n");
   }

   return 0;
}

#endif
