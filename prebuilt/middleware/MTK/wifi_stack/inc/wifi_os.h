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
#ifndef __WIFI_OS_H__
#define __WIFI_OS_H__

#if defined (KEIL_COMPILE)||(IAR_COMPILE)
#include "type_def.h"
#else
#include <sys/types.h>
#endif
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __WIFI_OS_H__
#define __WIFI_OS_H__

/****************************************************************************
 *
 * MESSAGE QUEUE
 *
 ****************************************************************************/

/**
 * create a message queue.
 *
 * @param message_size the size of a message.
 * @param queue_size the members that can be stored in the queue.
 * @return queue_id if >= 0, error number if < 0.
 */
typedef int32_t (*wifi_os_q_create)(uint32_t message_size, uint8_t queue_size);


/**
 * send a message via the message queue specifeid by queue_id.
 *
 * @param queue_id the ID of the queue to receive message from.
 * @param message the message to be sent.
 */
typedef void (*wifi_os_q_send)(int32_t queue_id, void *message);


/**
 * receve a message from the message queue specifeid by queue_id.
 *
 * @param queue_id the ID of the queue to receive message from.
 * @return the message from the queue.
 */
typedef void *(*wifi_os_q_recv)(int32_t queue_id);


/**
 * get an available message from queue.
 *
 * @param queue_id the ID of the queue to return the message to.
 * @return the pointer to the message if not full. null is returned if
 *         there is no more space.
 */
typedef void *(*wifi_os_q_alloc)(int32_t queue_id);


/**
 * return a processed message to the queue.
 *
 * @param queue_id the ID of the queue to return the message to.
 * @param message the ID of the queue to receive message from.
 */
typedef void (*wifi_os_q_free)(int32_t queue_id, void *message);


/****************************************************************************
 *
 * TIMER
 *
 ****************************************************************************/

#if 0
/*
 * The COS API is listed below. But I can't find any code actually uses TIMER
 * in Wi-Fi ROM/RAM code, only RF calibration and F/W download are using this
 * resource. Implement when needed.
 *
 * If implementation is needed, search for xTimerCreate(), xTimerStart(), and
 * xTimerChangePeriod() in FreeRTOS source code.
 */

void            cos_init_timer(void)

cos_timer_type* cos_create_timer(task_indx_type dest_id)

void            cos_start_timer(cos_timer_type *timer, kal_uint32 timeout);
void            cos_timer_expiration(void)
void            cos_stop_timer(cos_timer_type *timer)
void            cos_free_timer(void* timer_ptr)
kal_uint32      cos_get_systime(void)
#endif

/****************************************************************************
 *
 * SEMAPHORE
 *
 ****************************************************************************/

#if 0
/**
 * The Wi-Fi firmware is single threaded, with interrupts as the only
 * exception.
 *
 * When implementing ISRs, need to be careful of concurrent access to the
 * Wi-Fi firmware data structures. In this way, the semaphore would not be
 * needed.
 */
#endif

/****************************************************************************
 *
 * MEMORY
 *
 ****************************************************************************/


/**
 * see malloc(3) - Linux man page.
 */
typedef void *(*wifi_malloc)(size_t size);


/**
 * see free(3) - Linux man page.
 */
typedef void (*wifi_free)(void *ptr);


/****************************************************************************
 *
 * API FUNCTION POINTERS AGGREGATION
 *
 ****************************************************************************/

/**
 * a set of pointers of queue API. the usage is to allocate an instance of
 * wifi_os_q_api_t and provide corresponding implementation into the function
 * pointers. Then provide the aggregated function pointer set to the needed
 * module.
 */
typedef struct _wifi_os_api_t {
    /* MESSAGE QUEUE */
    wifi_os_q_create    q_create;
    wifi_os_q_send      q_enqueue;
    wifi_os_q_recv      q_dequeue;
    wifi_os_q_alloc     q_alloc;
    wifi_os_q_free      q_free;

    /* TIMER */

    /* SEMAPHORE */

    /* MEMORY */
    wifi_malloc         malloc;
    wifi_free           free;
} wifi_os_api_t;


/**
 * Ported API for FreeRTOS.
 */
extern wifi_os_api_t g_wifi_os_api_freertos;


#endif

#ifdef __cplusplus
}
#endif
#endif /* __WIFI_OS_H__ */