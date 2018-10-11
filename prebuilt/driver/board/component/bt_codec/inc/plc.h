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

#ifndef __PLC_H__
#define __PLC_H__

#include <stdint.h>

typedef struct {
    uint32_t sampling_rate;         /* Unit: Hz
                                       8000   --> Narrow Band Speech Signal
                                       16000  --> Wide Band Speech Signal
                                       Others --> Undefined */
} plc_initial_parameter_t;

typedef struct {
    uint32_t bad_frame_indicator;   /* 1      --> Bad Frame
                                       0      --> Good Frame
                                       Others --> Undefined */
    uint32_t bt_ev3_half_bad_flag;  /* 1      --> BT EV3 Packet Half Bad Frame
                                       0      --> Normal
                                       Others --> Undefined */
} plc_runtime_parameter_t;

/*
    Output Parameter
        internal_buffer_size    PLC internal buffer size, unit: byte
*/
uint32_t plc_get_buffer_size (void);

/*
    Input Parameters
        internal_buffer     PLC internal buffer
        initial_parameter   PLC initial parameters
    Output Parameters
        handle_pointer      pointer to the PLC handle
    Return Value
        >= 0    Normal
        < 0     Error
*/
int32_t  plc_init (void **handle_pointer, uint8_t *internal_buffer, plc_initial_parameter_t *initial_parameter);

/*
    Input Parameters
        handle              PLC handle
        pcm_data            pointer to the PCM buffer
        runtime_parameter   PLC runtime parameters
    Return Value
        >= 0    Normal
        < 0     Error
*/
int32_t  plc_process (void *handle, int16_t *pcm_data, plc_runtime_parameter_t *runtime_parameter);

#endif  /* __PLC_H__ */
