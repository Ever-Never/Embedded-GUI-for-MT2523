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

/* Common Internal Adapter API */
#include "atci_adapter.h"
/* Free RTOS */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include "hal_nvic_internal.h"


uint32_t atci_queue_create(uint32_t queue_length,uint32_t item_size)
{
    QueueHandle_t q_id = NULL;

    q_id = xQueueCreate(queue_length, item_size);
    return (uint32_t)q_id;
}

atci_status_t atci_queue_send(uint32_t q_id, void* data)
{
    BaseType_t ret = 0;
	BaseType_t xHigherPriorityTaskWoken;

	if ( 0 == q_id )
		return ATCI_STATUS_ERROR;
		
//	if( 0 == hal_nvic_query_exception_number())
	if( 0 == HAL_NVIC_QUERY_EXCEPTION_NUMBER)
    	ret = xQueueSend((QueueHandle_t)q_id, data, 0);
	else 
		ret = xQueueSendFromISR((QueueHandle_t)q_id, data, &xHigherPriorityTaskWoken);
		
    if (pdFAIL != ret) {
        return ATCI_STATUS_OK;
    } else {
        return ATCI_STATUS_ERROR;
    }
}


atci_status_t atci_queue_send_from_isr(uint32_t q_id, void* data) 
{
    BaseType_t ret = 0;
    BaseType_t xHigherPriorityTaskWoken;

    ret = xQueueSendFromISR((QueueHandle_t)q_id, data, &xHigherPriorityTaskWoken);
    if (pdPASS == ret) {
        return ATCI_STATUS_OK;
    } else {
        return ATCI_STATUS_ERROR;
    }
}


int32_t atci_queue_receive_no_wait(uint32_t q_id, void* data)
{
    BaseType_t ret = -1;
	if ( 0 == q_id )
		return 0;
    ret = xQueueReceive((QueueHandle_t)q_id, data, 0);
    return (int32_t)ret;
}

int32_t atci_queue_receive_wait(uint32_t q_id, void* data, uint32_t delay_time)
{
    BaseType_t ret = -1;
    ret = xQueueReceive((QueueHandle_t)q_id, data, delay_time / portTICK_PERIOD_MS);
    return (int32_t)ret;
}

uint16_t atci_queue_get_item_num(uint32_t q_id)
{
    uint16_t queue_item_num = 0;
	if ( 0 == q_id )
		return 0;
	
    queue_item_num = (uint16_t)uxQueueMessagesWaiting((QueueHandle_t) q_id);
    return queue_item_num;
}

uint32_t atci_mutex_create(void)
{
    return (uint32_t)xSemaphoreCreateMutex();
}

uint32_t atci_mutex_lock(uint32_t mutex_id)
{
	if(mutex_id == 0)
		return 0;
    return (uint32_t)xSemaphoreTake((SemaphoreHandle_t)mutex_id, portMAX_DELAY);
}

uint32_t atci_mutex_unlock(uint32_t mutex_id)
{
	if(mutex_id == 0)
		return 0;

    return (uint32_t)xSemaphoreGive((SemaphoreHandle_t)mutex_id);
}

void* atci_mem_alloc(uint32_t size)
{
    void *pvReturn = NULL;
    uint32_t  free_size;
    free_size = xPortGetFreeHeapSize();
    if (free_size > size) {
        pvReturn = pvPortMalloc(size);
    }

    return pvReturn;
}

void atci_mem_free(void *buf)
{
    if (buf != NULL) {
        vPortFree(buf);
    }
}

uint32_t atci_semaphore_create( uint32_t uxMaxCount, uint32_t uxInitialCount)
{
    return (uint32_t)xSemaphoreCreateCounting((UBaseType_t)uxMaxCount, (UBaseType_t)uxInitialCount);
}

uint32_t atci_semaphore_take(uint32_t semaphore_id)
{
	if ( NULL == (void *)semaphore_id )
		return 0;

    return (uint32_t)xSemaphoreTake((SemaphoreHandle_t)semaphore_id, portMAX_DELAY);
}

atci_status_t atci_semaphore_give(uint32_t semaphore_id)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if ( NULL == (void *)semaphore_id )
		return ATCI_STATUS_ERROR;
    //return (uint32_t)xSemaphoreGive((SemaphoreHandle_t)mutex_id);
    xSemaphoreGiveFromISR(semaphore_id, &xHigherPriorityTaskWoken);
	
	if(xHigherPriorityTaskWoken == pdTRUE) {
		return ATCI_STATUS_OK;

	} else {
		return ATCI_STATUS_ERROR;
	}
}


