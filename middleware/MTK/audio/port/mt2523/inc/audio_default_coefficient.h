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

#ifndef __AUDIO_DEFAULT_COEFFICIENT_H__
#define __AUDIO_DEFAULT_COEFFICIENT_H__



#ifdef __cplusplus
extern "C" {
#endif

#define SPEECH_COMMON_PARAMETER \
{ \
    6, 55997,31000,10752,32769,0,0,0,0,0,\
    0, 0\
    }


#define AUDIO_HANDSFREE_GAIN_STREAM_OUT_GAIN \
{ \
    /*-23,-21,-19,-17,-15,-13,-11,-9,-7,-5, \
    -3,-1,1,3,5,7 \*/ \
    0xFFE9, 0xFFEB, 0xFFED, 0xFFEF, 0xFFF1, 0xFFF3, 0xFFF5, 0xFFF7, 0xFFF9, 0xFFFB, \
    0xFFFD, 0xFFFF, 0x0001, 0x0003, 0x0005, 0x0007 \
}

#define AUDIO_HANDSFREE_GAIN_STREAM_IN_GAIN \
{ \
    0, \
}

#define AUDIO_HANDSFREE_GAIN_SIDETONE_GAIN 0


#define AUDIO_HEADSET_GAIN_STREAM_OUT_GAIN \
{ \
    /*
    -72, -68, -64, -60, -56, -52, -48, -44, -40, -36, \
    -32, -28, -24, -20, -16, -12\
    */ \
    0xFFB8, 0xFFBC, 0xFFC0, 0xFFC4, 0xFFC8, 0xFFCC, 0xFFD0, 0xFFD4, 0xFFD8, 0xFFDC, \
    0xFFE0, 0xFFE4, 0xFFE8, 0xFFEC, 0xFFF0, 0xFFF4 \
}

#define AUDIO_HEADSET_GAIN_STREAM_IN_GAIN \
{ \
    0, \
}

#define AUDIO_HEADSET_GAIN_SIDETONE_GAIN 0


#define AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER1 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER2 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER3 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER4 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER5 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER6 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER7 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER8 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER9 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER10 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE1_POST_PROCESSING_IIR_FILTER11 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER1 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER2 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER3 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER4 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER5 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER6 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER7 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER8 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER9 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER10 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE2_POST_PROCESSING_IIR_FILTER11 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER1 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER2 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER3 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER4 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER5 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER6 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER7 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER8 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER9 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER10 \
{ \
    0, 0, 0, 0, 0, \
}

#define AUDIO_ACF_MODE3_POST_PROCESSING_IIR_FILTER11 \
{ \
    0, 0, 0, 0, 0, \
}



#define VOICE_NB_MODE1_GAIN_STREAM_OUT_GAIN \
{ \
    /*
    -72, -68, -64, -60, -56, -52, -48, -44, -40, -36, \
    -32, -28, -24, -20, -16, -12\
    */ \
    0xFFB8, 0xFFBC, 0xFFC0, 0xFFC4, 0xFFC8, 0xFFCC, 0xFFD0, 0xFFD4, 0xFFD8, 0xFFDC, \
    0xFFE0, 0xFFE4, 0xFFE8, 0xFFEC, 0xFFF0, 0xFFF4 \
}

#define VOICE_NB_MODE1_GAIN_STREAM_IN_GAIN \
{ \
    30, \
}

#define VOICE_NB_MODE1_GAIN_SIDETONE_GAIN 0


#define VOICE_NB_MODE1_NB_MODE_PARAMETER \
{ \
      96,   253,  10756,    31, 53255,    10271,   400,     0,    80,  4293,\
     611,     0, 20488,     0,     0,    86,     0,     0,     0,     0,\
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
       0,     0,     0,     0,     0,     0,     0,     0,\
}

#define VOICE_NB_MODE1_NB_STREAM_IN_FIR_COEFFICIENT \
{ \
    32767, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, \
}

#define VOICE_NB_MODE1_NB_STREAM_IN_FIR_FREQUENCY \
{ \
    100, 200, 300, 400, 500, 1000, 2000, 3000, 4000, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_NB_MODE1_NB_STREAM_IN_FIR_RESPONSE \
{ \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_NB_MODE1_NB_STREAM_OUT_FIR_COEFFICIENT \
{ \
    32767, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, \
}

#define VOICE_NB_MODE1_NB_STREAM_OUT_FIR_FREQUENCY \
{ \
    100, 200, 300, 400, 500, 1000, 2000, 3000, 4000, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_NB_MODE1_NB_STREAM_OUT_FIR_RESPONSE \
{ \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_NB_MODE2_GAIN_STREAM_OUT_GAIN \
{ \
    /*-23,-21,-19,-17,-15,-13,-11,-9,-7,-5, \
    -3,-1,1,3,5,7 \*/ \
    0xFFE9, 0xFFEB, 0xFFED, 0xFFEF, 0xFFF1, 0xFFF3, 0xFFF5, 0xFFF7, 0xFFF9, 0xFFFB, \
    0xFFFD, 0xFFFF, 0x0001, 0x0003, 0x0005, 0x0007 \
}

#define VOICE_NB_MODE2_GAIN_STREAM_IN_GAIN \
{ \
    30, \
}

#define VOICE_NB_MODE2_GAIN_SIDETONE_GAIN 0


#define VOICE_NB_MODE2_NB_MODE_PARAMETER \
{ \
      96,   253,  10756,    31, 53255,    10271,   400,     0,    80,  4293,\
     611,     0, 20488,     0,     0,    86,     0,     0,     0,     0,\
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
       0,     0,     0,     0,     0,     0,     0,     0,\
}

#define VOICE_NB_MODE2_NB_STREAM_IN_FIR_COEFFICIENT \
{ \
    32767, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, \
}

#define VOICE_NB_MODE2_NB_STREAM_IN_FIR_FREQUENCY \
{ \
    100, 200, 300, 400, 500, 1000, 2000, 3000, 4000, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_NB_MODE2_NB_STREAM_IN_FIR_RESPONSE \
{ \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_NB_MODE2_NB_STREAM_OUT_FIR_COEFFICIENT \
{ \
    32767, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, \
}

#define VOICE_NB_MODE2_NB_STREAM_OUT_FIR_FREQUENCY \
{ \
    100, 200, 300, 400, 500, 1000, 2000, 3000, 4000, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_NB_MODE2_NB_STREAM_OUT_FIR_RESPONSE \
{ \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_NB_MODE3_GAIN_STREAM_OUT_GAIN \
{ \
    /*
    -72, -68, -64, -60, -56, -52, -48, -44, -40, -36, \
    -32, -28, -24, -20, -16, -12\
    */ \
    0xFFB8, 0xFFBC, 0xFFC0, 0xFFC4, 0xFFC8, 0xFFCC, 0xFFD0, 0xFFD4, 0xFFD8, 0xFFDC, \
    0xFFE0, 0xFFE4, 0xFFE8, 0xFFEC, 0xFFF0, 0xFFF4 \
}

#define VOICE_NB_MODE3_GAIN_STREAM_IN_GAIN \
{ \
    30, \
}

#define VOICE_NB_MODE3_GAIN_SIDETONE_GAIN 0


#define VOICE_NB_MODE3_NB_MODE_PARAMETER \
{ \
      96,   253,  10756,    31, 53255,    10271,   400,     0,    80,  4293,\
     611,     0, 20488,     0,     0,    86,     0,     0,     0,     0,\
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
       0,     0,     0,     0,     0,     0,     0,     0,\
}

#define VOICE_NB_MODE3_NB_STREAM_IN_FIR_COEFFICIENT \
{ \
    32767, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, \
}

#define VOICE_NB_MODE3_NB_STREAM_IN_FIR_FREQUENCY \
{ \
    100, 200, 300, 400, 500, 1000, 2000, 3000, 4000, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_NB_MODE3_NB_STREAM_IN_FIR_RESPONSE \
{ \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_NB_MODE3_NB_STREAM_OUT_FIR_COEFFICIENT \
{ \
    32767, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, \
}

#define VOICE_NB_MODE3_NB_STREAM_OUT_FIR_FREQUENCY \
{ \
    100, 200, 300, 400, 500, 1000, 2000, 3000, 4000, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_NB_MODE3_NB_STREAM_OUT_FIR_RESPONSE \
{ \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_WB_MODE1_GAIN_STREAM_OUT_GAIN \
{ \
    /*
    -72, -68, -64, -60, -56, -52, -48, -44, -40, -36, \
    -32, -28, -24, -20, -16, -12\
    */ \
    0xFFB8, 0xFFBC, 0xFFC0, 0xFFC4, 0xFFC8, 0xFFCC, 0xFFD0, 0xFFD4, 0xFFD8, 0xFFDC, \
    0xFFE0, 0xFFE4, 0xFFE8, 0xFFEC, 0xFFF0, 0xFFF4 \
}

#define VOICE_WB_MODE1_GAIN_STREAM_IN_GAIN \
{ \
    30, \
}

#define VOICE_WB_MODE1_GAIN_SIDETONE_GAIN 0


#define VOICE_WB_MODE1_WB_MODE_PARAMETER \
{ \
      96,   253,  10756,    31, 53255,    10271,   400,     0,    80,  4293,\
     611,     0, 20488,     0,     0,    86,     0,     0,     0,     0,\
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
       0,     0,     0,     0,     0,     0,     0,     0,\
}

#define VOICE_WB_MODE1_WB_STREAM_IN_FIR_COEFFICIENT \
{ \
    32767, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
}

#define VOICE_WB_MODE1_WB_STREAM_IN_FIR_FREQUENCY \
{ \
    100, 200, 300, 400, 500, 1000, 2000, 3000, 4000, 7500, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_WB_MODE1_WB_STREAM_IN_FIR_RESPONSE \
{ \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_WB_MODE1_WB_STREAM_OUT_FIR_COEFFICIENT \
{ \
    32767, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
}

#define VOICE_WB_MODE1_WB_STREAM_OUT_FIR_FREQUENCY \
{ \
    100, 200, 300, 400, 500, 1000, 2000, 3000, 4000, 7500, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_WB_MODE1_WB_STREAM_OUT_FIR_RESPONSE \
{ \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_WB_MODE2_GAIN_STREAM_OUT_GAIN \
{ \
    /*-23,-21,-19,-17,-15,-13,-11,-9,-7,-5, \
    -3,-1,1,3,5,7 \*/ \
    0xFFE9, 0xFFEB, 0xFFED, 0xFFEF, 0xFFF1, 0xFFF3, 0xFFF5, 0xFFF7, 0xFFF9, 0xFFFB, \
    0xFFFD, 0xFFFF, 0x0001, 0x0003, 0x0005, 0x0007 \
}

#define VOICE_WB_MODE2_GAIN_STREAM_IN_GAIN \
{ \
    30, \
}

#define VOICE_WB_MODE2_GAIN_SIDETONE_GAIN 0


#define VOICE_WB_MODE2_WB_MODE_PARAMETER \
{ \
      96,   253,  10756,    31, 53255,    10271,   400,     0,    80,  4293,\
     611,     0, 20488,     0,     0,    86,     0,     0,     0,     0,\
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
       0,     0,     0,     0,     0,     0,     0,     0,\
}

#define VOICE_WB_MODE2_WB_STREAM_IN_FIR_COEFFICIENT \
{ \
    32767, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
}

#define VOICE_WB_MODE2_WB_STREAM_IN_FIR_FREQUENCY \
{ \
    100, 200, 300, 400, 500, 1000, 2000, 3000, 4000, 7500, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_WB_MODE2_WB_STREAM_IN_FIR_RESPONSE \
{ \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_WB_MODE2_WB_STREAM_OUT_FIR_COEFFICIENT \
{ \
    32767, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
}

#define VOICE_WB_MODE2_WB_STREAM_OUT_FIR_FREQUENCY \
{ \
    100, 200, 300, 400, 500, 1000, 2000, 3000, 4000, 7500, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_WB_MODE2_WB_STREAM_OUT_FIR_RESPONSE \
{ \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_WB_MODE3_GAIN_STREAM_OUT_GAIN \
{ \
    /*
    -72, -68, -64, -60, -56, -52, -48, -44, -40, -36, \
    -32, -28, -24, -20, -16, -12\
    */ \
    0xFFB8, 0xFFBC, 0xFFC0, 0xFFC4, 0xFFC8, 0xFFCC, 0xFFD0, 0xFFD4, 0xFFD8, 0xFFDC, \
    0xFFE0, 0xFFE4, 0xFFE8, 0xFFEC, 0xFFF0, 0xFFF4 \
}

#define VOICE_WB_MODE3_GAIN_STREAM_IN_GAIN \
{ \
    30, \
}

#define VOICE_WB_MODE3_GAIN_SIDETONE_GAIN 0


#define VOICE_WB_MODE3_WB_MODE_PARAMETER \
{ \
      96,   253,  10756,    31, 53255,    10271,   400,     0,    80,  4293,\
     611,     0, 20488,     0,     0,    86,     0,     0,     0,     0,\
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,\
       0,     0,     0,     0,     0,     0,     0,     0,\
}

#define VOICE_WB_MODE3_WB_STREAM_IN_FIR_COEFFICIENT \
{ \
    32767, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
}

#define VOICE_WB_MODE3_WB_STREAM_IN_FIR_FREQUENCY \
{ \
    100, 200, 300, 400, 500, 1000, 2000, 3000, 4000, 7500, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_WB_MODE3_WB_STREAM_IN_FIR_RESPONSE \
{ \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_WB_MODE3_WB_STREAM_OUT_FIR_COEFFICIENT \
{ \
    32767, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
}

#define VOICE_WB_MODE3_WB_STREAM_OUT_FIR_FREQUENCY \
{ \
    100, 200, 300, 400, 500, 1000, 2000, 3000, 4000, 7500, \
    0, 0, 0, 0, 0, 0, \
}

#define VOICE_WB_MODE3_WB_STREAM_OUT_FIR_RESPONSE \
{ \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, \
}


#define RECORD_WB_MODE1_GAIN_STREAM_OUT_GAIN \
{ \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE1_GAIN_STREAM_IN_GAIN \
{ \
    30, \
}

#define RECORD_WB_MODE1_GAIN_SIDETONE_GAIN 0


#define RECORD_WB_MODE1_WB_MODE_PARAMETER \
{ \
    0, 479, 16388, 36892, 37127, 8192, 400, 0, 272, 4293, \
    611,  0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE1_WB_CHANNEL1_FIR_COEFFICIENT \
{ \
    32767, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE1_WB_CHANNEL1_FIR_FREQUENCY \
{ \
    100, 200, 300, 400, 500, 1000, 2000, 3000, 4000, 7500, \
    0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE1_WB_CHANNEL1_FIR_RESPONSE \
{ \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE1_WB_CHANNEL2_FIR_COEFFICIENT \
{ \
    32767, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE1_WB_CHANNEL2_FIR_FREQUENCY \
{ \
    100, 200, 300, 400, 500, 1000, 2000, 3000, 4000, 7500, \
    0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE1_WB_CHANNEL2_FIR_RESPONSE \
{ \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE2_GAIN_STREAM_OUT_GAIN \
{ \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE2_GAIN_STREAM_IN_GAIN \
{ \
    30, \
}

#define RECORD_WB_MODE2_GAIN_SIDETONE_GAIN 0


#define RECORD_WB_MODE2_WB_MODE_PARAMETER \
{ \
    0, 479, 16388, 36892, 37127, 8192, 400, 0, 272, 4293, \
    611,  0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE2_WB_CHANNEL1_FIR_COEFFICIENT \
{ \
    32767, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE2_WB_CHANNEL1_FIR_FREQUENCY \
{ \
    100, 200, 300, 400, 500, 1000, 2000, 3000, 4000, 7500, \
    0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE2_WB_CHANNEL1_FIR_RESPONSE \
{ \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE2_WB_CHANNEL2_FIR_COEFFICIENT \
{ \
    32767, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE2_WB_CHANNEL2_FIR_FREQUENCY \
{ \
    100, 200, 300, 400, 500, 1000, 2000, 3000, 4000, 7500, \
    0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE2_WB_CHANNEL2_FIR_RESPONSE \
{ \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE3_GAIN_STREAM_OUT_GAIN \
{ \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE3_GAIN_STREAM_IN_GAIN \
{ \
    30, \
}

#define RECORD_WB_MODE3_GAIN_SIDETONE_GAIN 0


#define RECORD_WB_MODE3_WB_MODE_PARAMETER \
{ \
    0, 479, 16388, 36892, 37127, 8192, 400, 0, 272, 4293, \
    611,  0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE3_WB_CHANNEL1_FIR_COEFFICIENT \
{ \
    32767, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE3_WB_CHANNEL1_FIR_FREQUENCY \
{ \
    100, 200, 300, 400, 500, 1000, 2000, 3000, 4000, 7500, \
    0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE3_WB_CHANNEL1_FIR_RESPONSE \
{ \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE3_WB_CHANNEL2_FIR_COEFFICIENT \
{ \
    32767, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE3_WB_CHANNEL2_FIR_FREQUENCY \
{ \
    100, 200, 300, 400, 500, 1000, 2000, 3000, 4000, 7500, \
    0, 0, 0, 0, 0, 0, \
}

#define RECORD_WB_MODE3_WB_CHANNEL2_FIR_RESPONSE \
{ \
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 0, 0, \
}


#ifdef __cplusplus
}
#endif

#endif /*__AUDIO_DEFAULT_COEFFICIENT_H__*/

