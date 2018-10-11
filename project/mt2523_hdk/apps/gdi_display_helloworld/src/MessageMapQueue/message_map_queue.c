#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "task_call.h"

#include "syslog.h"

#include "message.h"
#include "message_map_queue.h"
#include "memory_attribute.h"


static SemaphoreHandle_t LinkListMutex;

static struct QueueList_t Queuelist[MESSAGE_LIST_END];

/******************************************************************************/
/** message map queue mechanism initialize
  *
  * @return none
  *
  * @note
  *         this function is only called at main() before any task was created.
  *
  *//*
  * History        :
  * 1.Date         : 2016/8/23
  *   Author       : bruce.yin
  *   Modification : Created function

*******************************************************************************/
ATTR_TEXT_IN_TCM int LinkListInit(void)
{
    int msg;

    for(msg = 0; msg < MESSAGE_LIST_END; msg++)
    {
        Queuelist[msg].queue = NULL;
    }

    LinkListMutex = xSemaphoreCreateMutex();

    return 1;
}

/******************************************************************************/
/** malloc a new queue node and insert it to the QueueList_t
  *
  * @param[in] listx the list to insert , the listx : Queuelist[]->queue
  * @param[in] queue new queue node to insert to the QueueList_t
  *
  * @return
  *     - < 0 the queue inserted failed
  *     - > 0 the queue inserted success
  * @note
  *
  *//*
  * History        :
  * 1.Date         : 2016/9/4
  *   Author       : bruce.yin
  *   Modification : Created function

*******************************************************************************/
ATTR_TEXT_IN_TCM int LinkListInsert(struct QueueList_t * listx, QueueHandle_t queue)
{
    int exist = 0;

    struct QueueLinkNode * cur_node = NULL, * new_node = NULL;

    if(NULL == listx)
    {
        return -1;
    }

    cur_node = listx->queue;

    while(NULL != cur_node)
    {
        new_node = cur_node;

        if(queue == cur_node->queue)
        {
            exist = 1;

            break;
        }

        cur_node = cur_node->next_queue;
    }

    if(!exist)
    {
        cur_node = (struct QueueLinkNode * )pvPortMalloc(sizeof(struct QueueLinkNode));

        if(NULL == cur_node)
        {
            return -1;
        }

        cur_node->queue      = queue;
        cur_node->next_queue = NULL;

        new_node->next_queue = cur_node;
    }

    return 1;
}

/******************************************************************************/
/** delete a specified queue node from the QueueList
  *
  * @param[in] listx the list to insert , the listx : Queuelist[]->queue
  * @param[in] queue new queue node to insert to the QueueList_t
  *
  * @return
  *     - < 0 the queue isn't present
  *     - > 0 the queue present and delete success
  * @note
  *
  *//*
  * History        :
  * 1.Date         : 2016/9/4
  *   Author       : bruce.yin
  *   Modification : Created function

*******************************************************************************/
ATTR_TEXT_IN_TCM int LinkListDelete(struct QueueList_t * listx, QueueHandle_t queue)
{
    int exist = 0;

    struct QueueLinkNode * cur_node = NULL, * prv_node = NULL;

    if((NULL == listx)||(NULL == queue))
    {
        return -1;
    }

    cur_node = listx->queue;

    while( NULL != cur_node )
    {
        if(queue == cur_node->queue)
        {
            exist = 1;

            break;
        }

        prv_node = cur_node;

        cur_node = cur_node->next_queue;
    }

    if(exist)
    {
        if(cur_node == listx->queue)
        {
            listx->queue = cur_node->next_queue;
        }
        else
        {
            prv_node->next_queue = cur_node->next_queue;
        }

        vPortFree(cur_node);

        return 1;
    }

    return -1;
}

/******************************************************************************/
/** to construct the map relation between message and queue.
  *
  * @param[in] msg   the message to map to the queue < queue, msg >
  * @param[in] queue the queue to map the msg < queue, msg >
  *
  * @return
  *     - < 0 the queue inserted failled , and the map is canceled.
  *     - > 0 the queue is inserted success, and the map relation is ok
  * @note
  *
  *//*
  * History        :
  * 1.Date         : 2016/9/4
  *   Author       : bruce.yin
  *   Modification : Created function

*******************************************************************************/
ATTR_TEXT_IN_TCM int message_queue_map(int msg, QueueHandle_t * queue)
{
    struct QueueLinkNode * new_node;

    if(msg < MESSAGE_LIST_END)
    {
        xSemaphoreTake(LinkListMutex, portMAX_DELAY);

        if(NULL == Queuelist[msg].queue)
        {
            new_node = (struct QueueLinkNode *)pvPortMalloc(sizeof(struct QueueLinkNode));

            if(NULL == new_node)
            {
                xSemaphoreGive(LinkListMutex);
                return -1;
            }

            new_node->queue = queue;
            new_node->next_queue = NULL;

            Queuelist[msg].queue = new_node;
        }
        else
        {
            LinkListInsert(&Queuelist[msg], queue);
        }

        xSemaphoreGive(LinkListMutex);
        return 1;
    }

    return -1;
}

/******************************************************************************/
/** when dellete a task , then delete the message and queue map relation
  *
  * @param[in] msg   the message to unmap with the spec-queue
  * @param[in] queue the queue to unmap with the msg
  *
  * @return
  *     - < 0 the <queue, msg> unmap failed, and the unmap operation is failed
  *     - > 0 the <queue, msg> unmap success, and the unmap operation is OK
  * @note
  *
  *//*
  * History        :
  * 1.Date         : 2016/9/4
  *   Author       : bruce.yin
  *   Modification : Created function

*******************************************************************************/
ATTR_TEXT_IN_TCM int message_queue_unmap(int msg, QueueHandle_t queue)
{
    /* you can delete msg-queue-map directly, but make sure the queue be empty before delete task !!! */

    int v;

    xSemaphoreTake(LinkListMutex, portMAX_DELAY);

    /* must examine if the queue is empty,code is andded later */

    v = LinkListDelete(&Queuelist[msg], queue);

    xSemaphoreGive(LinkListMutex);

    return v;
}

/******************************************************************************/
/** message send function interupt security version
  *
  * @param[in] send_msg: the message to send
  * @param[in] para_pointer: the local data
  * @param[in] pxHigherPriorityTaskWoken: if interupt exit
  *
  * @return
  *     - < 0 the message send failed
  *     - > 0 the message send success
  * @note
  *     this function is similar to the MessageSend , but it only support deliver
  *     VALUE, because the ISR no need malloc()function,and the malloc isn't
  *     reentrant, so it dosen't security in the interupt call.
  *
  *
  *//*
  * History        :
  * 1.Date         : 2016/9/4
  *   Author       : bruce.yin
  *   Modification : Created function

*******************************************************************************/
ATTR_TEXT_IN_TCM int MessageSendFromISR(int send_msg, uint32_t para_pointer, BaseType_t *pxHigherPriorityTaskWoken)
{
               MESSAGE_T   message;
              BaseType_t   send_result;
    struct QueueLinkNode * queue_root = NULL;

    if(send_msg > MESSAGE_LIST_END)
    {
        return -1;
    }

    queue_root = Queuelist[send_msg].queue;/* get the message's root queue node */

    while(( NULL != queue_root )&&(NULL != queue_root->queue))
    {
        message.parameter.value = para_pointer;

        message.message_id = send_msg | MESSAGE_IS_VALUE;

        /* dispacth message data */
        send_result = xQueueSendFromISR( queue_root->queue, &message, pxHigherPriorityTaskWoken);

        if( errQUEUE_FULL == send_result )
        {
            return -1;
        }

        queue_root = queue_root->next_queue; /* next queue */
    }

    return 1;
}

/******************************************************************************/
/** message send function
  *
  * @param[in] send_msg: the message to send
  * @param[in] para_pointer: the local data buffer address
  * @param[in] para_length: the length of local data to send
  * @param[in] pointer_or_value: MESSAGE_IS_POINTER  or MESSAGE_IS_VALUE
  *
  * @return
  *     - < 0 the message send failed
  *     - > 0 the message send success
  * @note
  *     when use this function, the caller no need use memory manangment function
  *     such as: malloc(), pvPortMalloc(), because this function do the memory
  *     manangement malloc and free , the message data which pointed by para_pointer
  *     will be copied to the internel malloc buffer, and the send the message to
  *     the queue.
  *
  *//*
  * History        :
  * 1.Date         : 2016/9/4
  *   Author       : bruce.yin
  *   Modification : Created function

*******************************************************************************/
ATTR_TEXT_IN_TCM int MessageSend(int send_msg, void * para_pointer, uint32_t para_length, uint32_t pointer_or_value)
{
               MESSAGE_T   message;
              BaseType_t   send_result;
    struct QueueLinkNode * queue_root = NULL;

    assert(para_pointer);

    if(send_msg > MESSAGE_LIST_END)
    {
        return -1;
    }

    queue_root = Queuelist[send_msg].queue;/* get the message's root queue node */

    while(( NULL != queue_root )&&(NULL != queue_root->queue))
    {
        if(para_length > MESSAGE_MAX_LENGTH)
        {
            return -1;
        }

        /* package message data */
        if( MESSAGE_IS_POINTER == pointer_or_value )/* deliver pointer */
        {
            message.parameter.pointer = pvPortMalloc( para_length + 4);

            if(NULL == message.parameter.pointer)
            {
                return -1;
            }

            *(uint32_t *)message.parameter.pointer = para_length;/* data feild length */

            memcpy((uint8_t *)message.parameter.pointer + 4, (uint8_t *)para_pointer, para_length);/* make sure 'para_length >= 4' */

            message.message_id = send_msg | MESSAGE_IS_POINTER;
        }
        else/* deliver value */
        {
            message.parameter.value = *(uint32_t *)para_pointer;

            message.message_id = send_msg | MESSAGE_IS_VALUE;
        }

        /* dispacth message data */
        send_result = xQueueSend( queue_root->queue, &message, portMAX_DELAY /* (portTickType)10 */);

        if( errQUEUE_FULL == send_result )
        {
            return -1;
        }

        queue_root = queue_root->next_queue; /* next queue */
    }

    return 1;
}

/******************************************************************************/
/** message recv function
  *
  * @param[in] queue: the message to send
  * @param[in] received_msg: the receive message ID will be copied to received_msg
  * @param[in] received_msg_data: the receive message data will be copied to
  *                               received_msg_data
  *
  * @return
  *     - < 0 the message send failed
  *     - > 0 the message send success
  * @note
  *     when use this function, the caller no need use memory manangment function
  *     such as: free(), vPortFree(), because this function do the memory
  *     manangement free auto ,
  *
  *     cation:
  *     this function do not give the message data's poniter ,it will copy the
  *     malloc data buffer the local buffer where local buffer is received_msg_data
  *
  *//*
  * History        :
  * 1.Date         : 2016/9/4
  *   Author       : bruce.yin
  *   Modification : Created function

*******************************************************************************/
ATTR_TEXT_IN_TCM int MessageRecv(QueueHandle_t queue, int * const received_msg, void * received_msg_data)
{
    MESSAGE_T  message;

    assert(queue);
    assert(received_msg);
    assert(received_msg_data);

    if( pdFALSE == xQueueReceive( queue, &message, portMAX_DELAY ))
    {
        return -1;  /* if error , do nothing and return */
    }

    /* get the received message structure */
    if( MESSAGE_IS_VALUE == ( message.message_id & MESSAGE_IS_VALUE )) /* delivered value */
    {
        *received_msg = message.message_id - MESSAGE_IS_VALUE; /* msg id */

        *(uint32_t *)received_msg_data = message.parameter.value /* msg data */;

        return 1;
    }
    else /* delivered pointer */
    {
        *received_msg = message.message_id;

        if( NULL != message.parameter.pointer )/* need to check the pointer's validation */
        {
            if(*(uint32_t *)message.parameter.pointer <= MESSAGE_MAX_LENGTH)
            {
                memcpy((uint8_t *)received_msg_data, (uint8_t *)message.parameter.pointer + 4, *(uint32_t *)message.parameter.pointer );

                vPortFree( message.parameter.pointer );/* release the pointer */
                return 1;
            }
            else
            {
                vPortFree( message.parameter.pointer );/* release the pointer */
                return -1;
            }
        }
    }

    return -1;
}

ATTR_TEXT_IN_TCM int MessageRecvTimeout(QueueHandle_t queue, int *const received_msg, void *received_msg_data, int ms)
{
    MESSAGE_T message;

    assert(queue);
    assert(received_msg);
    assert(received_msg_data);
    TickType_t delay_time = pdMS_TO_TICKS(ms);
    if (pdFALSE == xQueueReceive(queue, &message, delay_time))
    {
        return -1; /* if error , do nothing and return */
    }

    /* get the received message structure */
    if (MESSAGE_IS_VALUE == (message.message_id & MESSAGE_IS_VALUE)) /* delivered value */
    {
        *received_msg = message.message_id - MESSAGE_IS_VALUE; /* msg id */

        *(uint32_t *)received_msg_data = message.parameter.value /* msg data */;

        return 1;
    }
    else /* delivered pointer */
    {
        *received_msg = message.message_id;

        if (NULL != message.parameter.pointer) /* need to check the pointer's validation */
        {
            if (*(uint32_t *)message.parameter.pointer <= MESSAGE_MAX_LENGTH)
            {
                memcpy((uint8_t *)received_msg_data, (uint8_t *)message.parameter.pointer + 4, *(uint32_t *)message.parameter.pointer);

                vPortFree(message.parameter.pointer); /* release the pointer */
                return 1;
            }
            else
            {
                vPortFree(message.parameter.pointer); /* release the pointer */
                return -1;
            }
        }
    }

    return -1;
}

