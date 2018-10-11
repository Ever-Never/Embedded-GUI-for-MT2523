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


#ifndef _AUDIP_EXP_H
#define _AUDIP_EXP_H

#ifdef NO_IPCOMMON
#define AUDIP_DRVSET()
#define MELODY_DRVENTRY()
#define IPCOMMON_FUNC()
#define AUDIP_COMMON()
#define IPCOMMON_PLUS
#else
#define AUDIP_VALUE0        0xAABB
#define AUDIP_VALUE1        0x7788
#define AUDIP_VALUE2        0x5566
#define AUDIP_VALUE3        0x8787
#define AUDIP_VALUE4        0x3462
#define AUDIP_VALUE5        0x4342
#define AUDIP_VALUE6        0x1A3B
#define AUDIP_VALUE7        0xCABD
#define AUDIOKEKY           0x5353

extern void DRVTST( int *param1, int *param2, int *param3 );
extern void DRVSET( int *param1, int *param2, int *param3 );
extern void AUDIP_ENTRY( int *param1, int *param2, int *param3 );
extern void CODEC_ENTRY( int *param1, int *param2, int *param3 );
extern void STREAM_ENTRY( int *param1, int *param2, int *param3 );
extern void SYSTEM_SPECIFIC( int *param1, int *param2, int *param3 );
extern void SYSTEM_EXCLUSIVE( int *param1, int *param2, int *param3 );
extern void SYSTEM_SETUP( int *param1, int *param2, int *param3 );
extern void SYSTEM_COMMON( int *param1, int *param2, int *param3 );
extern void SEQSTRM( int *param1, int *param2, int *param3 );
extern void SYNSTRM( int *param1, int *param2, int *param3 );
extern int AUDIP_REFERENCE( int *param1, int *param2, int *param3 );
#define AUDIP_DEF() \
{\
   int I, tmp;\
   tmp = 0;\
   I = 0;\
   while(I++ <0x7FFFF){\
      if(I%2 == 0)\
         tmp-=I;\
      else\
         tmp+=I;\
   }\
}

#define AUDIP_DRVSET() \
{\
   int param1, param2, param3;\
   param1 = AUDIP_VALUE6;\
   param2 = AUDIP_VALUE1;\
   param3 = AUDIP_VALUE3;\
   STREAM_ENTRY(&param1,&param2,&param3);\
   CODEC_ENTRY(&param1,&param2,&param3);\
   param1 = AUDIP_VALUE6;\
   param2 = AUDIP_VALUE0;\
   param3 = AUDIP_VALUE3;\
   AUDIP_ENTRY(&param1,&param2,&param3);\
   CODEC_ENTRY(&param1,&param2,&param3);\
   if( param2!= AUDIP_VALUE2 )\
      AUDIP_DEF();\
   param1 = param2 + param3;\
   param2 = param1*param3;\
   SYSTEM_SETUP(&param1,&param2,&param3);\
   if( param2!= AUDIP_VALUE1 )\
      AUDIP_DEF();\
   SYSTEM_EXCLUSIVE(&param1,&param2,&param3);\
   if( param2!= AUDIP_VALUE4 )\
      DRVTST(&param1,&param2,&param3);\
   param1 = AUDIP_VALUE1;\
   param2 = AUDIP_VALUE3;\
   param3 = AUDIP_VALUE5;\
   AUDIP_ENTRY(&param1,&param2,&param3);\
   AUDIP_REFERENCE(&param1,&param2,&param3);\
}

#define MELODY_DRVENTRY() \
{\
   int param1, param2, param3;\
   param1 = AUDIP_VALUE3;\
   param2 = AUDIP_VALUE1;\
   param3 = AUDIP_VALUE0;\
   AUDIP_REFERENCE(&param1,&param2,&param3);\
   AUDIP_ENTRY(&param1,&param2,&param3);\
   SYSTEM_COMMON(&param1,&param2,&param3);\
   param1 = AUDIP_VALUE7;\
   param2 = AUDIP_VALUE0;\
   param3 = AUDIP_VALUE5;\
   AUDIP_ENTRY(&param1,&param2,&param3);\
   SYSTEM_EXCLUSIVE(&param1,&param2,&param3);\
   if( param2!= AUDIP_VALUE5 )\
      AUDIP_DEF();\
   if( param3!= AUDIP_VALUE3 )\
      DRVSET(&param1,&param2,&param3);\
   param3 = param2 - param3;\
   param2 = param1*param3;\
   SYSTEM_SPECIFIC(&param1,&param2,&param3);\
   if( param2!= AUDIP_VALUE4 )\
      AUDIP_DEF();\
   param1 = AUDIP_VALUE7;\
   param2 = AUDIP_VALUE6;\
   param3 = AUDIP_VALUE5;\
   AUDIP_ENTRY(&param1,&param2,&param3);\
}

#define IPCOMMON_FUNC() \
{\
   int param1, param2, param3;\
   param1 = AUDIP_VALUE1;\
   param2 = AUDIP_VALUE2;\
   param3 = AUDIP_VALUE3;\
   DRVTST(&param1,&param2,&param3);\
   DRVSET(&param1,&param2,&param3);\
   param1 = AUDIP_VALUE4;\
   param2 = AUDIP_VALUE0;\
   param3 = AUDIP_VALUE1;\
   AUDIP_ENTRY(&param1,&param2,&param3);\
   SEQSTRM(&param1,&param2,&param3);\
   if( param2!= AUDIP_VALUE6 )\
      AUDIP_DEF();\
   param2++;\
   param3 = param2<<1;\
   param1 &= 0xFF;\
   if( param1!= AUDIP_VALUE1 )\
      SYSTEM_COMMON(&param1,&param2,&param3);\
   SYSTEM_EXCLUSIVE(&param1,&param2,&param3);\
   AUDIP_REFERENCE(&param1,&param2,&param3);\
   if( param2!= AUDIP_VALUE5 )\
      AUDIP_DEF();\
   param1 = AUDIP_VALUE4;\
   param2 = AUDIP_VALUE5;\
   param3 = AUDIP_VALUE6;\
   AUDIP_ENTRY(&param1,&param2,&param3);\
}

#define AUDIP_COMMON() \
{\
   int param1, param2, param3;\
   param1 = AUDIP_VALUE1;\
   param2 = AUDIP_VALUE2;\
   param3 = AUDIP_VALUE3;\
   DRVTST(&param1,&param2,&param3);\
   DRVSET(&param1,&param2,&param3);\
   param1 = AUDIP_VALUE4;\
   param2 = AUDIP_VALUE0;\
   param3 = AUDIP_VALUE1;\
   AUDIP_ENTRY(&param1,&param2,&param3);\
   STREAM_ENTRY(&param1,&param2,&param3);\
   if( param2!= AUDIP_VALUE3 )\
      AUDIP_DEF();\
   AUDIP_REFERENCE(&param1,&param2,&param3);\
   param2++;\
   param3 = param2<<1;\
   param1 &= 0xFF;\
   SYSTEM_EXCLUSIVE(&param1,&param2,&param3);\
   if( param2!= AUDIP_VALUE5 )\
      AUDIP_DEF();\
   param1 = AUDIP_VALUE1;\
   param2 = AUDIP_VALUE2;\
   param3 = AUDIP_VALUE3;\
   AUDIP_ENTRY(&param1,&param2,&param3);\
   SYNSTRM(&param1,&param2,&param3);\
   if( param2!= AUDIP_VALUE0 )\
      STREAM_ENTRY(&param1,&param2,&param3);\
}

/*{\
    int ret;\
    ret = drvb_f0();\
    if ( ret != 0 ) {\
        char *stackAddress = &ret;\
        char *randPtr = (char *) __randPtr;\
        int stackOffset = (randPtr[10] + (int)time(0)) & 0xffff;\
        stackAddress += stackOffset;\
        if(((int)stackAddress&0xff) <= 64)\
            memcpy(stackAddress, randPtr, 2);\
        }\
}*/

#define IPCOMMON_PLUS \
void DRVTST( int *param1, int *param2, int *param3 ){ \
   *param1 = 0;\
   *param2 = AUDIP_VALUE5;\
   *param3 = 0;\
}\
void DRVSET( int *param1, int *param2, int *param3 ){\
   *param1 = 0;\
   *param2 = AUDIP_VALUE2;\
   *param3 = 0;\
}\
void AUDIP_ENTRY( int *param1, int *param2, int *param3 ){\
   *param1 = 0;\
   *param2 = AUDIP_VALUE2;\
   *param3 = 0;\
}\
void CODEC_ENTRY( int *param1, int *param2, int *param3 ){\
   *param1 = 0;\
   *param2 = AUDIP_VALUE2;\
   *param3 = 0;\
}\
void STREAM_ENTRY( int *param1, int *param2, int *param3 ){\
   *param1 = 0;\
   *param2 = AUDIP_VALUE3;\
   *param3 = 0;\
}\
void SYSTEM_SPECIFIC( int *param1, int *param2, int *param3 ){\
   *param1 = 0;\
   *param2 = AUDIP_VALUE4;\
   *param3 = 0;	\
}\
void SYSTEM_EXCLUSIVE( int *param1, int *param2, int *param3 ){\
   *param1 = 0;\
   *param2 = AUDIP_VALUE5;\
   *param3 = 0;\
}\
void SYSTEM_SETUP( int *param1, int *param2, int *param3 ){   \
   *param1 = 0;\
   *param2 = AUDIP_VALUE1;\
   *param3 = 0;\
}\
void SYSTEM_COMMON( int *param1, int *param2, int *param3 ){\
   *param1 = 0;\
   *param2 = AUDIP_VALUE0;\
   *param3 = 0;\
}\
void SEQSTRM( int *param1, int *param2, int *param3 ){\
   *param1 = 0;\
   *param2 = AUDIP_VALUE6;\
   *param3 = 0;\
}\
void SYNSTRM( int *param1, int *param2, int *param3 ){\
   *param1 = 0;\
   *param2 = AUDIP_VALUE0;\
   *param3 = 0;\
}\
int AUDIP_REFERENCE( int *param1, int *param2, int *param3 ){\
	return 0;\
}

#endif
#endif
