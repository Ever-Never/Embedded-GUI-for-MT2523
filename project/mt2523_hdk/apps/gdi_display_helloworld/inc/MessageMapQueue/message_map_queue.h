#ifndef MSG_MAP_QUEUE_H
#define MSG_MAP_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "task_call.h"
#include "message.h"
#include "memory_attribute.h"    

struct QueueLinkNode
{
    QueueHandle_t queue;

    struct QueueLinkNode * next_queue;
};

struct QueueList_t
{   
    struct QueueLinkNode * queue;    
};    

#define MESSAGE_IS_VALUE     (1 << 30) 
#define MESSAGE_IS_POINTER   (0 << 30)
//#define MESSAGE_MAX_LENGTH   (256    )
#define MESSAGE_MAX_LENGTH   (256*3 )


ATTR_TEXT_IN_TCM int MessageSendFromISR(int send_msg, uint32_t para_pointer, BaseType_t *pxHigherPriorityTaskWoken);
ATTR_TEXT_IN_TCM int MessageSend( int send_msg, void * para_pointer, uint32_t para_length, uint32_t pointer_or_value );
ATTR_TEXT_IN_TCM int MessageRecv( QueueHandle_t queue, int * const received_msg, void * received_msg_data );
ATTR_TEXT_IN_TCM int MessageRecvExt(QueueHandle_t queue, int * const received_msg, void ** received_msg_data_pointer);
ATTR_TEXT_IN_TCM int MessageRecvTimeout(QueueHandle_t queue, int * const received_msg, void * received_msg_data , int ms) ;
ATTR_TEXT_IN_TCM int message_queue_map(int msg, QueueHandle_t * queue);
ATTR_TEXT_IN_TCM int message_queue_unmap(int msg, QueueHandle_t queue);

ATTR_TEXT_IN_TCM int LinkListInit(void);
    
#ifdef __cplusplus
}
#endif    

#endif
