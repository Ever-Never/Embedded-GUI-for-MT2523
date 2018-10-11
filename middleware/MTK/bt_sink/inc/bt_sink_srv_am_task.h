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

#ifndef __AM_MAIN_H__
#define __AM_MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "queue.h"

#define AM_RING_BUFFER_LEN   (16*1024)
#define AM_RING_BUFFER_SIZE  (AM_RING_BUFFER_LEN)


#define AM_ERR_SUCCESS_7TH            (7)
#define AM_ERR_SUCCESS_6TH            (6)
#define AM_ERR_SUCCESS_5TH            (5)
#define AM_ERR_SUCCESS_4TH            (4)
#define AM_ERR_SUCCESS_3RD            (3)
#define AM_ERR_SUCCESS_2ND            (2)
#define AM_ERR_SUCCESS_1ST            (1)
#define AM_ERR_SUCCESS_OK             (0)
#define AM_ERR_FAIL_1ST               (-1)
#define AM_ERR_FAIL_2ND               (-2)
#define AM_ERR_FAIL_3RD               (-3)
#define AM_ERR_FAIL_4TH               (-4)
#define AM_ERR_FAIL_5TH               (-5)
#define AM_ERR_FAIL_6TH               (-6)
#define AM_ERR_FAIL_7TH               (-7)


#ifndef WIN32_UT
#define AM_TASK_STACK_SIZE   (512)
#define AM_TASK_PRIORITY     (6)
#define AM_EXPIRE_TIMER_MAX  (10)
#define AM_TIMER_PERIOD      (500)
#define AM_TIMER_ID          (1)
extern QueueHandle_t g_xQueue_am;
#endif

#ifndef WIN32_UT
void am_task_create(void);
void am_task_main(void *pvParameters);
#else
void am_task_main(void);
#endif

#ifdef RTOS_TIMER
void aud_timer_callback(TimerHandle_t pxTimer);
#endif

#ifdef __cplusplus
}
#endif

#endif /*__AM_MAIN_H__*/

