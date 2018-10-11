#ifndef MESSAGE_H
#define MESSAGE_H

#ifdef __cplusplus
    extern "C" {
#endif
        
//#include "ble_message.h"
/**
  * - task and interupt message enum list.
  *  - Message enum struct, all message used by tasks and interupt must be defined here.
  */        
typedef enum
{
    MSG_NULL = 0,
	#include "task_display_message.h"
    #if 0
	#include "task_rtc_message.h"
	#include "task_sensor_message.h"
	#include "task_gnss_message.h"
	#include "task_battery_message.h"
	#include "task_ble_message.h"
	#include "task_system_message.h"
	#include "task_storage_message.h"
	
    #include "task_usb_message.h"
    #include "task_demo_message.h"
    #include "task_gnss_message.h"
    #include "task_memory_message.h"
    #include "task_factory_message.h"
    #endif    
    MESSAGE_LIST_END /** max message count */
    
} MESSAGE_E;

/** 
  * parameter of message struct
  * - 1. *pointer: point a message parameter which is greater than 4 bytes.
  * - 2. value: if your message parameter less than 4 bytes, assign the parameter to the value directly.
  *
  * @brief
  *      if the parameter is greater than 4 bytes, you should define a struct.  About the struct, 
  *      please refer to software architecture files.
  */
typedef union
{
    void * pointer; /** transmit pointer */
    uint32_t value; /** transmit value   */
    
} PARAMETER_U;

/**
  * - message_id : identification of message, one of the enum struct: MESSAGE_E
  * - PARAMETER_U: parameter union struct, which can be a uint32_t value, or a pointer that point
  *                a parameter value struct.
  */
typedef struct
{
    int32_t message_id;
    PARAMETER_U parameter;
    
} MESSAGE_T;


#ifdef __cplusplus
    }
#endif

#endif
