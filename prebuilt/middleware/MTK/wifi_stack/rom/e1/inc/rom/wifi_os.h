#ifdef __cplusplus
extern "C" {
#endif

#ifndef __WIFI_OS_H__
#define __WIFI_OS_H__
#include "cm4_type.h"

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
kal_uint32      cos_get_clock_ticks(void)
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

#if 0
/*****************************************
 * Moved from os_util.h
 *
 *****************************************/
 
typedef long os_time_t;


struct os_time {
    os_time_t sec;
    os_time_t usec;
};


struct os_tm {
    int sec; /* 0..59 or 60 for leap seconds */
    int min; /* 0..59 */
    int hour; /* 0..23 */
    int day; /* 1..31 */
    int month; /* 1..12 */
    int year; /* Four digit year */
};
#endif

/**
 * Ported API for FreeRTOS.
 */
extern int (*rom_sprintf)( char * str, const char * format, ... );
extern int (*iot_rom_printf)(const char *fmt, ...);
extern void (*rom_hex_dump)(char *str, unsigned char *pSrcBufVA, unsigned int SrcBufLen);
extern void * (*iot_malloc)(size_t size);
extern void (*iot_free)(void *ptr);
extern unsigned int (*cos_get_clock_ticks)();
extern void (*cos_delay_clock_ticks)(unsigned int ticks);
extern void (*RTMPInitTimer)(void *pAd, void *pTimer, void *func, void *pData, char repeat, const char *const pcTimerName) ;
extern void (*RTMPSetTimer)(void *pTimer, unsigned long timeout);
extern void (*RTMPModTimer)(void *pTimer, unsigned long value);
extern void (*RTMPCancelTimer)(void *prTimer, unsigned char *pCancelled);
extern void (*RTMPDeleteTimer)(void *prTimer);
extern void (*update_beacon_process)(int bStart);
extern void (*NdisGetSystemUpTime)(unsigned long *time);
extern int (*wifi_firmware_inband_queue_query_boson)(
    unsigned char   cmd_id,
    unsigned char   *buf,
    ssize_t         len,
    unsigned char   *ret_buf,
    ssize_t         out_len,
    unsigned char   is_from_isr,
    unsigned char   is_ext_cmd);
extern int (*wifi_firmware_inband_queue_exec_boson)(
    unsigned char   cmd_id,
    unsigned char   *buf,
    ssize_t         len,
    unsigned char   is_from_isr,
    unsigned char   is_ext_cmd);
extern int (*wifi_firmware_inband_query_tx_frame_boson)(
    unsigned char   cmd_id,
    unsigned char   *buf,
    ssize_t         len,
    unsigned char   is_query,
    unsigned char   is_ext_cmd);
#ifndef assert
#define assert(x) {while(1);}
#endif // not define assert

#endif // WIFI_OS_H

#ifdef __cplusplus
}
#endif
