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

#if !defined (MTK_DEBUG_LEVEL_NONE)

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "syslog.h"
#include "toi.h"
#include "hal_gpt.h"
#include "exception_handler.h"
#include "timers.h"
#if defined(MTK_PORT_SERVICE_ENABLE)
#include "serial_port.h"
#endif
#include "task_def.h"

#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)
#include "string.h"
#include "hal_flash.h"
#include "flash_map.h"

#endif /* MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE */

/* max size of each block of normal log message */
#define MAX_LOG_SIZE (256)

/* max size of each block of hex dump message */
#define MAX_DUMP_MESSAGE_SIZE (128)

/* max size of each block of hex dump message */
#define MAX_DUMP_DATA_SIZE (128)

/* max hex dump length */
#define MAX_HEXDUMP_LENGTH (2048)

#if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697) || (PRODUCT_VERSION == 7686) || (PRODUCT_VERSION == 7682) || (PRODUCT_VERSION == 5932) || defined(MTK_NO_PSRAM_ENABLE)

/* buffer pool items */
#define LOG_POOL_ITEMS (SYSLOG_QUEUE_LENGTH + 8)

/* maximum delay to wait for available log queue */
#define LOG_QUEUE_WAITING_TIME (portMAX_DELAY)

#elif (PRODUCT_VERSION == 2523) || (PRODUCT_VERSION == 2533)

/* For MT2523, the bt audio related feature generates lots of bt logs. */

/* buffer pool items */
#define LOG_POOL_ITEMS SYSLOG_QUEUE_LENGTH

/* maximum delay to wait for available log queue */
#define LOG_QUEUE_WAITING_TIME (0)

#endif

#define change_level_to_string(level) \
  ((level) - PRINT_LEVEL_DEBUG <= PRINT_LEVEL_ERROR) ? print_level_table[level] : "debug"

typedef enum {
    MESSAGE_TYPE_NORMAL_LOG,
    MESSAGE_TYPE_HEX_DUMP
} message_type_t;

typedef struct {
    const char *module_name;
    print_level_t print_level;
    const char *func_name;
    int line_number;
    uint32_t timestamp;
    char message[MAX_LOG_SIZE + 1]; //C string format
} normal_log_message_t;

typedef struct {
    const char *module_name;
    print_level_t print_level;
    const char *func_name;
    int line_number;
    uint32_t timestamp;
    char message[MAX_DUMP_MESSAGE_SIZE + 1]; //C string format
    uint32_t data_length;
    char data[MAX_DUMP_DATA_SIZE];
    char *additional_data;
} hex_dump_message_t;


/* log message definition */
typedef struct {
    message_type_t message_type;
    union {
        normal_log_message_t normal_log_message;
        hex_dump_message_t   hex_dump_message;
    } u;
} log_message_t;

typedef struct {
    uint32_t occupied;
    uint8_t  buffer[sizeof(log_message_t)];
} syslog_buffer_t;

/* syslog task handle */
static xTaskHandle syslog_task_handle;

/* syslog buffer pool */
static syslog_buffer_t syslog_buffer_pool[LOG_POOL_ITEMS];

/* log queue handle */
static xQueueHandle g_log_queue = NULL;

#define SYSLOG_UART_INIT_DONE 0x1
#define SYSLOG_GPT_INIT_DONE  0x2
#define SYSLOG_NVDM_INIT_DONE 0x4
#define SYSLOG_UART_DMA_MODE  0x8
#define SYSLOG_PORT_OPEN_DONE 0x10

static uint32_t syslog_init_time = 0;
static uint32_t syslog_init_done = 0;
static uint32_t syslog_task_ready = false;
static uint32_t syslog_drop_count = 0;

/* create common module for unspecified module to use */
log_create_module(common, PRINT_LEVEL_INFO);

static syslog_config_t syslog_config;

static const char *log_switch_table[]  = { "on", "off" };

static const char *print_level_table[] = { "debug", "info", "warning", "error" };

static hal_uart_port_t g_log_uart_port;

#if defined(MTK_PORT_SERVICE_ENABLE)
static serial_port_dev_t g_log_port;
static serial_port_handle_t g_log_port_handle;
static SemaphoreHandle_t tx_sem, rx_sem;
void syslog_serial_port_switch(serial_port_dev_t device);
#endif

#if  defined ( __GNUC__ )
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
#endif /* __GNUC__ */

/*******************************************************************************
 *  syslog uart port management
 ******************************************************************************/

hal_uart_status_t log_uart_init(hal_uart_port_t port)
{
    hal_uart_config_t uart_config;
    hal_uart_status_t ret;

    /* Configure UART PORT */
    uart_config.baudrate = HAL_UART_BAUDRATE_115200;
    uart_config.parity = HAL_UART_PARITY_NONE;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;

    g_log_uart_port = port;
    ret = hal_uart_init(port, &uart_config);

    syslog_init_done |= SYSLOG_UART_INIT_DONE;

    return ret;
}

void log_putchar(char byte)
{
    hal_uart_put_char(g_log_uart_port, byte);
}

#if defined (MTK_PORT_SERVICE_ENABLE)

__weak serial_port_status_t serial_port_open(serial_port_dev_t device,
                                             serial_port_open_para_t *para,
                                             serial_port_handle_t *handle)
{
    configASSERT(device - SERIAL_PORT_DEV_UART_0 <= SERIAL_PORT_DEV_UART_3);
    /* If port service feature is not enabled, use the one passed in log_uart_init. */
    return (log_uart_init(g_log_uart_port) == HAL_UART_STATUS_OK) ? SERIAL_PORT_STATUS_OK: SERIAL_PORT_STATUS_FAIL;
}

__weak serial_port_status_t serial_port_close(serial_port_handle_t handle)
{
    return SERIAL_PORT_STATUS_OK;
}

__weak serial_port_status_t serial_port_config_read_dev_number(const char *user_name, serial_port_dev_t *device)
{
    return SERIAL_PORT_STATUS_OK;
}

__weak serial_port_status_t serial_port_config_write_dev_number(const char *user_name, serial_port_dev_t device)
{
    return SERIAL_PORT_STATUS_OK;
}

__weak serial_port_status_t serial_port_control(serial_port_handle_t handle,
                                                serial_port_ctrl_cmd_t command,
                                                serial_port_ctrl_para_t *para)
{
    return SERIAL_PORT_STATUS_OK;
}

static void log_switch_to_polling_mode(void)
{
    syslog_init_done &= ~(SYSLOG_UART_DMA_MODE);
    log_uart_init(g_log_uart_port);
}

static void log_switch_to_dma_mode(void)
{
    syslog_init_done |= SYSLOG_UART_DMA_MODE | SYSLOG_PORT_OPEN_DONE;
}

static void exception_io_init(void)
{
    log_switch_to_polling_mode();
    syslog_task_ready = false;
}

int log_write(char *buf, int len)
{
    serial_port_status_t status;
    serial_port_ctrl_para_t send_data;
    int ret=0;
    int i;

    if (syslog_init_done & SYSLOG_UART_INIT_DONE)  {
        send_data.serial_port_write_data.data = (uint8_t*)buf;
        send_data.serial_port_write_data.size = len;

        if (syslog_init_done & SYSLOG_UART_DMA_MODE) {
            while(1) {
                status = serial_port_control(g_log_port_handle, SERIAL_PORT_CMD_WRITE_DATA, &send_data);
                if (status == SERIAL_PORT_STATUS_OK) {
                    send_data.serial_port_write_data.size -= send_data.serial_port_write_data.ret_size;
                    send_data.serial_port_write_data.data += send_data.serial_port_write_data.ret_size;
                }
                if (send_data.serial_port_write_data.size == 0) {
                    break;
                }
                if ((xTaskGetSchedulerState() != taskSCHEDULER_SUSPENDED) && ((portNVIC_INT_CTRL_REG & 0xff)==0)) {
                    xSemaphoreTake(tx_sem, 1000);
                }
            }
            ret = (send_data.serial_port_write_data.size == 0) ? len : 0;
        } else {
            if (syslog_init_done & SYSLOG_PORT_OPEN_DONE) {
                status = serial_port_control(g_log_port_handle, SERIAL_PORT_CMD_WRITE_DATA_BLOCKING, &send_data);
                ret = (status == SERIAL_PORT_STATUS_OK) ? len : 0;
            }
            else {
                for (i = 0; i < len; i++) {
                    log_putchar(*buf++);
                }
                ret = len;
            }
        }
    }
    return ret;
}

static void syslog_port_service_callback(serial_port_dev_t device, serial_port_callback_event_t event, void *parameter)
{
    BaseType_t xHigherPriorityTaskWoken;

    switch(event) {
        case SERIAL_PORT_EVENT_READY_TO_READ:
            xSemaphoreGiveFromISR(rx_sem, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
        break;

        case SERIAL_PORT_EVENT_READY_TO_WRITE:
            xSemaphoreGiveFromISR(tx_sem, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
        break;

        case SERIAL_PORT_EVENT_USB_CONNECTION:
            if (g_log_port == device) {
                log_switch_to_dma_mode();
            }
        break;

        case SERIAL_PORT_EVENT_USB_DISCONNECTION:
            if (g_log_port == device) {
                g_log_port = (serial_port_dev_t)g_log_uart_port;
                syslog_serial_port_switch(g_log_port);
            }
        break;

        default:
        break;
    }
}

#endif /* MTK_PORT_SERVICE_ENABLE */

#define LOG_TS(p) (((log_message_t*)(p)->buffer)->u.normal_log_message.timestamp)

static syslog_buffer_t *sort_syslog_buffer(syslog_buffer_t *pList)
{
    syslog_buffer_t *pHead = NULL, *pCurr, *p;

    if (pList == NULL || pList->occupied == 0) {
        return pList;
    }

    while (pList != NULL) {
        pCurr = pList;
        pList = (syslog_buffer_t*)(pList->occupied);
        if ((pHead == NULL) || LOG_TS(pCurr) < LOG_TS(pHead)) {
           pCurr->occupied = (uint32_t)pHead;
           pHead = pCurr;
        }
        else {
           p = pHead;
           while (p != NULL) {
              if ((p->occupied == 0) ||
                  LOG_TS(pCurr) < LOG_TS((syslog_buffer_t*)(p->occupied))) {
                  pCurr->occupied = p->occupied;
                  p->occupied = (uint32_t)pCurr;
                  break;
              }
              p = (syslog_buffer_t*)(p->occupied);
           }
        }
    }
    return pHead;
}

void process_log_message(const log_message_t *log_message);

#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)

void save_normal_log(uint32_t offset, const normal_log_message_t *normal_log_message)
{
    printf("[T: %u M: %s C: %s F: %s L: %d]: ",
           (unsigned int)normal_log_message->timestamp,
           normal_log_message->module_name,
           change_level_to_string(normal_log_message->print_level),
           normal_log_message->func_name,
           normal_log_message->line_number);
    printf("%s\r\n", (char *)normal_log_message->message);
}


static uint32_t save_log_message(uint32_t offset, const log_message_t *log_message)
{
    const normal_log_message_t *normal_log_message;
    hal_flash_status_t status;
    int hlen, mlen;
    uint8_t buf[MAX_LOG_SIZE+1];
	uint32_t syslog_buffer_base = CRASH_SYSLOG_BASE;
	
    /* only support normal log in saving crash context */
    if (MESSAGE_TYPE_NORMAL_LOG == log_message->message_type) {
        normal_log_message = &log_message->u.normal_log_message;
        hlen = snprintf((char*)buf, MAX_LOG_SIZE,
                        "\r\n[T: %u M: %s C: %s F: %s L: %d]: ",
                        (unsigned int)normal_log_message->timestamp,
                        normal_log_message->module_name,
                        change_level_to_string(normal_log_message->print_level),
                        normal_log_message->func_name,
                        normal_log_message->line_number);
        status = hal_flash_write(syslog_buffer_base + offset, buf, hlen);
        mlen = strlen(normal_log_message->message);
        status = hal_flash_write(syslog_buffer_base + offset + hlen, (uint8_t*)(normal_log_message->message), mlen);
        (void)status;
        return (hlen + mlen);
    }
    return 0;
}

static uint32_t try_save_assert_expr(uint32_t offset)
{
    const char *expr, *file;
    hal_flash_status_t status;
    int line, len=0;
    uint8_t buf[MAX_LOG_SIZE+1];
    uint32_t syslog_buffer_base = CRASH_SYSLOG_BASE;
    exception_get_assert_expr(&expr, &file, &line);
    if (expr) {
        len = snprintf((char*)buf, MAX_LOG_SIZE,
                       "\n\rassert failed: %s, file: %s, line: %d\n\r",
                       expr, file, line);
        status = hal_flash_write(syslog_buffer_base + offset, buf, len);
        (void)status;
    }
    return len;
}

static void syslog_buffer_dump(void)
{
    uint32_t index, offset;
    syslog_buffer_t *pHead = NULL, *pCurr;
    hal_flash_status_t status;

    /* erase the flash blocks reserved for coredump */
    for (offset = 0; offset < CRASH_SYSLOG_LENGTH; offset += 0x1000) {
        status  = hal_flash_erase(CRASH_SYSLOG_BASE + offset, HAL_FLASH_BLOCK_4K);
        (void)status;
        /* printf("coredump erase flash: offset=0x%08x, status=%d\n\r", (unsigned int)offset, status); */
    }

    /* mark all used entries as in use */
    for (index = 0; index < LOG_POOL_ITEMS; index++) {
        if (((log_message_t*)(syslog_buffer_pool[index].buffer))->u.normal_log_message.module_name) {
            syslog_buffer_pool[index].occupied = 1;
        }
    }

    /* create a link list of syslog buffer(s) in use now */
    for (index = 0; index < LOG_POOL_ITEMS; index++) {
        /* reuse the 'occupied' field as a pointer */
        if (syslog_buffer_pool[index].occupied) {
            syslog_buffer_pool[index].occupied = (uint32_t)pHead;
            pHead = &syslog_buffer_pool[index];
        }
    }

    /* sort the log by time */
    pHead = sort_syslog_buffer(pHead);

    printf("\n\r>>> dump syslog buffer\n\r\n\r");

    offset = 0;
    /* print the sorted log */
    while (pHead) {
        pCurr = pHead;
        pHead = (syslog_buffer_t*)(pHead->occupied);
        pCurr->occupied = 1;
        offset += save_log_message(offset, (log_message_t *)(pCurr->buffer));
        process_log_message((log_message_t *)(pCurr->buffer));
        if (offset + MAX_LOG_SIZE*2 > CRASH_SYSLOG_LENGTH) {
            break;
        }
    }

    try_save_assert_expr(offset);
}


#else

static void syslog_buffer_dump(void)
{
    uint32_t index;
    syslog_buffer_t *pHead = NULL, *pCurr;

    /* create a link list of syslog buffer(s) in use now */
    for (index = 0; index < LOG_POOL_ITEMS; index++) {
        /* reuse the 'occupied' field as a pointer */
        if (syslog_buffer_pool[index].occupied) {
            syslog_buffer_pool[index].occupied = (uint32_t)pHead;
            pHead = &syslog_buffer_pool[index];
        }
    }

    /* sort the log by time */
    pHead = sort_syslog_buffer(pHead);

    printf("\n\r>>> dump syslog buffer\n\r\n\r");

    /* print the sorted log */
    while (pHead) {
        pCurr = pHead;
        pHead = (syslog_buffer_t*)(pHead->occupied);
        pCurr->occupied = 1;
        process_log_message((log_message_t *)(pCurr->buffer));
    }
}

#endif

static void syslog_exception_cb(void)
{
#if defined(MTK_PORT_SERVICE_ENABLE)
    exception_io_init();
#endif
    syslog_buffer_dump();
}

/*******************************************************************************
 *  syslog memory pool management
 ******************************************************************************/
void buffer_pool_init()
{
    memset(syslog_buffer_pool, 0, sizeof(syslog_buffer_pool));
}

#define DEBUG_SYSLOG_BLOCK_ISSUE 0

#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)

static uint32_t next_buffer_index = 0;

syslog_buffer_t *buffer_pool_alloc(void)
{
    UBaseType_t uxSavedInterruptStatus;
    uint32_t i, index;

    uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
    for (i = 0; i < LOG_POOL_ITEMS; i++) {
        index = (next_buffer_index + i) % LOG_POOL_ITEMS;
        if (!syslog_buffer_pool[index].occupied) {
            syslog_buffer_pool[index].occupied = true;
            next_buffer_index = (index + 1) % LOG_POOL_ITEMS;
            portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );
            return &syslog_buffer_pool[index];
        }
    }
    syslog_drop_count++;
    portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );

#if (DEBUG_SYSLOG_BLOCK_ISSUE == 1)
    configASSERT(syslog_drop_count < 3000);
#endif

    return NULL;
}

#else

syslog_buffer_t *buffer_pool_alloc(void)
{
    UBaseType_t uxSavedInterruptStatus;
    uint32_t index;

    uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
    for (index = 0; index < LOG_POOL_ITEMS; index++) {
        if (!syslog_buffer_pool[index].occupied) {
            syslog_buffer_pool[index].occupied = true;
            portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );
            return &syslog_buffer_pool[index];
        }
    }
    syslog_drop_count++;
    portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );

#if (DEBUG_SYSLOG_BLOCK_ISSUE == 1)
    configASSERT(syslog_drop_count < 3000);
#endif

    return NULL;
}

#endif

void buffer_pool_release(syslog_buffer_t *syslog_buffer)
{
    UBaseType_t uxSavedInterruptStatus;
    log_message_t *log_message = (log_message_t *)syslog_buffer->buffer;

    uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
    syslog_buffer->occupied = false;
    if ((log_message->message_type == MESSAGE_TYPE_HEX_DUMP)
            && (log_message->u.hex_dump_message.additional_data) ) {
        vPortFree(log_message->u.hex_dump_message.additional_data);
    }
    portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );
}

static unsigned int get_current_time_in_ms(void)
{
    uint32_t count = 0;
    uint64_t count64 = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &count);
    count64 = ((uint64_t)count)*1000/32768;
    return (unsigned int)count64;
}

/*******************************************************************************
 *  syslog queue management
 ******************************************************************************/
void log_queue_create(void)
{
    g_log_queue = xQueueCreate(SYSLOG_QUEUE_LENGTH, sizeof(syslog_buffer_t *));
    if (NULL == g_log_queue) {
        printf("log_queue_create: create log queue failed.\r\n");
    }
}

void log_queue_send(syslog_buffer_t **log_buffer)
{
    if (portNVIC_INT_CTRL_REG & 0xff) {
        BaseType_t xTaskWokenByPost = pdFALSE;
        if (xQueueSendFromISR(g_log_queue, log_buffer, &xTaskWokenByPost) ==  errQUEUE_FULL ) {
            buffer_pool_release(*log_buffer);
        } else {
            portYIELD_FROM_ISR(xTaskWokenByPost);
        }
    } else {
        xQueueSend(g_log_queue, log_buffer, LOG_QUEUE_WAITING_TIME);
    }
}

int log_queue_receive(syslog_buffer_t **log_buffer)
{
    BaseType_t ret_val = xQueueReceive(g_log_queue, log_buffer, portMAX_DELAY);
    return (pdPASS == ret_val) ? 0 : -1;
}

/*******************************************************************************
 *  syslog implementation
 ******************************************************************************/

#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)



#define LOG_BUFFER_COUNT 3
#define FLASH_BLOCK_SIZE 4096

typedef char syslog_dump_buffer_t[FLASH_BLOCK_SIZE];

static uint32_t g_curr_write_block = NORMAL_SYSLOG_BASE;
static uint32_t g_curr_position = 0, g_avail_buffer_count = LOG_BUFFER_COUNT;
static uint32_t g_curr_write_buffer = 0, g_curr_read_buffer = 0;
static syslog_dump_buffer_t *g_syslog_buffer_array;
static char g_one_log_buffer[2048 + 256];

static bool syslog_switch_uart_to_flash = false; 
bool syslog_switch_from_uart_to_flash()
{
  return syslog_switch_uart_to_flash;
}

uint8_t syslog_switch_cmds(uint8_t len, char *param[])
{

    if ( len < 1 ) {
        printf("<syslog_switch_from_uart_to_flash>   := on | off\n");
    }

	if(strcmp(param[0],"on") == 0) {
        syslog_switch_uart_to_flash = true; 
	  	printf("\n\r this is flash mode\n\r");
	} 

	if(strcmp(param[0],"off") == 0) { 
		syslog_switch_uart_to_flash = false;  
		printf("\n\rthis is uart mode\n\r");
        
	}  

	return 0;
  
}


static uint32_t print_normal_log_variant(const normal_log_message_t *normal_log_message, char *log_buffer, uint32_t buffer_size)
{
    uint32_t curr_index;

    curr_index = 0;
    curr_index += snprintf(log_buffer, buffer_size,
                           "[T: %u M: %s C: %s F: %s L: %d]: ",
                           (unsigned int)normal_log_message->timestamp,
                           normal_log_message->module_name,
                           change_level_to_string(normal_log_message->print_level),
                           normal_log_message->func_name,
                           normal_log_message->line_number);
    curr_index += snprintf(&log_buffer[curr_index], (buffer_size - curr_index),
                           "%s\r\n",
                           (char *)normal_log_message->message);

    return curr_index;
}

static uint32_t hex_dump_data_variant(const char *data, int length, char *log_buffer, uint32_t buffer_size)
{
    int index = 0;
    uint32_t curr_index;

    curr_index = 0;
    for (index = 0; index < length; index++) {
        curr_index += snprintf(&log_buffer[curr_index], (buffer_size - curr_index), "%02X", (int)(data[index]));
        if ((index + 1) % 16 == 0) {
            curr_index += snprintf(&log_buffer[curr_index], (buffer_size - curr_index), "\r\n");
            continue;
        }
        if (index + 1 != length) {
            curr_index += snprintf(&log_buffer[curr_index], (buffer_size - curr_index), " ");
        }
    }
    if (0 != index && 0 != index % 16) {
        curr_index += snprintf(&log_buffer[curr_index], (buffer_size - curr_index), "\r\n"); //add one more blank line
    }

    return curr_index;
}

static uint32_t print_hex_dump_variant(const hex_dump_message_t *hex_dump_message, char *log_buffer, uint32_t buffer_size)
{
    uint32_t curr_index;

    /* format the syslog */
    curr_index = 0;
    curr_index += snprintf(log_buffer, buffer_size,
                           "[T: %u M: %s C: %s F: %s L: %d]: ",
                           (unsigned int)hex_dump_message->timestamp,
                           hex_dump_message->module_name,
                           change_level_to_string(hex_dump_message->print_level),
                           hex_dump_message->func_name,
                           hex_dump_message->line_number);
    curr_index += snprintf(&log_buffer[curr_index], (buffer_size - curr_index),
                           "%s\r\n",
                           (char *)hex_dump_message->message);
    if (hex_dump_message->data_length > MAX_DUMP_DATA_SIZE) {
        curr_index += hex_dump_data_variant(hex_dump_message->data,
                                            MAX_DUMP_DATA_SIZE,
                                            &log_buffer[curr_index],
                                            (buffer_size - curr_index));
        curr_index += hex_dump_data_variant(hex_dump_message->additional_data,
                                            hex_dump_message->data_length - MAX_DUMP_DATA_SIZE,
                                            &log_buffer[curr_index],
                                            (buffer_size - curr_index));
    } else {
        curr_index += hex_dump_data_variant(hex_dump_message->data,
                                            hex_dump_message->data_length,
                                            &log_buffer[curr_index],
                                            (buffer_size - curr_index));
    }

    return curr_index;
}

static void log_write_to_flash(void)
{
    uint32_t i, full_buffer_count;

    full_buffer_count = LOG_BUFFER_COUNT - g_avail_buffer_count;
    for (i = 0; i < full_buffer_count; i++) {
        hal_flash_erase(g_curr_write_block, HAL_FLASH_BLOCK_4K);
        hal_flash_write(g_curr_write_block, (uint8_t *)g_syslog_buffer_array[g_curr_read_buffer], FLASH_BLOCK_SIZE);
        if (++g_curr_read_buffer >= LOG_BUFFER_COUNT) {
            g_curr_read_buffer = 0;
        }
        g_curr_write_block += FLASH_BLOCK_SIZE;
        if (g_curr_write_block >= (NORMAL_SYSLOG_BASE + NORMAL_SYSLOG_LENGTH)) {
            g_curr_write_block = NORMAL_SYSLOG_BASE;
        }
    }
    g_avail_buffer_count = LOG_BUFFER_COUNT;
}

static void log_from_queue_to_buffer(char *log_buffer, uint32_t count)
{
    uint32_t left_space;

    left_space = FLASH_BLOCK_SIZE - g_curr_position;
    if (count <= left_space) {
        /* buffer can fill with this logging message. */
        memcpy(&g_syslog_buffer_array[g_curr_write_buffer][g_curr_position], log_buffer, count);
        g_curr_position += count;
    } else {
        /* If no buffer is available, trigger writting to flash. */
        if (g_avail_buffer_count == 0) {
            log_write_to_flash();
        }
        /* Now buffer is available again. */
        g_avail_buffer_count--;
        if (++g_curr_write_buffer >= LOG_BUFFER_COUNT) {
            g_curr_write_buffer = 0;
        }
        g_curr_position = 0;
        memset(g_syslog_buffer_array[g_curr_write_buffer], 0, FLASH_BLOCK_SIZE);
        memcpy(&g_syslog_buffer_array[g_curr_write_buffer][g_curr_position], log_buffer, count);
        g_curr_position += count;
    }
}

static void process_log_message_variant(const log_message_t *log_message)
{
    uint32_t count;

    if (syslog_drop_count) {
        UBaseType_t uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();;
        count = snprintf(g_one_log_buffer, sizeof(g_one_log_buffer), "\t\t>>> log drop count = %u\r\n\n", (unsigned int)syslog_drop_count);
        log_from_queue_to_buffer(g_one_log_buffer, count);
        syslog_drop_count = 0;
        portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );
    }

    if (MESSAGE_TYPE_NORMAL_LOG == log_message->message_type) {
        count = print_normal_log_variant(&log_message->u.normal_log_message, g_one_log_buffer, sizeof(g_one_log_buffer));
    } else {
        count = print_hex_dump_variant(&log_message->u.hex_dump_message, g_one_log_buffer, sizeof(g_one_log_buffer));
    }
    log_from_queue_to_buffer(g_one_log_buffer, count);

    /* if no message in queue, trigger writting to flash. */
    if (uxQueueMessagesWaiting(g_log_queue) == 0) {
        log_write_to_flash();
    }
}

int32_t log_dump_buffer_alloc(int32_t buffer_size)
{
    int32_t real_size;

    /* 4k align size of buffer */
    real_size = (buffer_size + FLASH_BLOCK_SIZE - 1) / FLASH_BLOCK_SIZE;
    real_size = (real_size * FLASH_BLOCK_SIZE) * 2;

    g_syslog_buffer_array = (syslog_dump_buffer_t *)pvPortMalloc(real_size*2);
    if (g_syslog_buffer_array == NULL) {
        return -1;
    }

    return real_size*2;
}

#endif

void print_normal_log(const normal_log_message_t *normal_log_message)
{
    printf("[T: %u M: %s C: %s F: %s L: %d]: ",
           (unsigned int)normal_log_message->timestamp,
           normal_log_message->module_name,
           change_level_to_string(normal_log_message->print_level),
           normal_log_message->func_name,
           normal_log_message->line_number);
    printf("%s\r\n", (char *)normal_log_message->message);
}

void hex_dump_data(const char *data, int length)
{
    int index = 0;
    for (index = 0; index < length; index++) {
        printf("%02X", (int)(data[index]));
        if ((index + 1) % 16 == 0) {
            printf("\r\n");
            continue;
        }
        if (index + 1 != length) {
            printf(" ");
        }
    }
    if (0 != index && 0 != index % 16) {
        printf("\r\n");//add one more blank line
    }
}

void print_hex_dump(const hex_dump_message_t *hex_dump_message)
{
    printf("[T: %u M: %s C: %s F: %s L: %d]: ",
           (unsigned int)hex_dump_message->timestamp,
           hex_dump_message->module_name,
           change_level_to_string(hex_dump_message->print_level),
           hex_dump_message->func_name,
           hex_dump_message->line_number);
    printf("%s\r\n", (char *)hex_dump_message->message);
    if (hex_dump_message->data_length > MAX_DUMP_DATA_SIZE) {
        hex_dump_data(hex_dump_message->data, MAX_DUMP_DATA_SIZE);
        hex_dump_data(hex_dump_message->additional_data, hex_dump_message->data_length - MAX_DUMP_DATA_SIZE);
    } else {
        hex_dump_data(hex_dump_message->data, hex_dump_message->data_length);
    }
}

void process_log_message(const log_message_t *log_message)
{
    if (syslog_drop_count) {
        UBaseType_t uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();;
        printf("\t\t>>> log drop count = %u\r\n\n", (unsigned int)syslog_drop_count);
        syslog_drop_count = 0;
        portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );
    }

    if (MESSAGE_TYPE_NORMAL_LOG == log_message->message_type) {
        print_normal_log(&log_message->u.normal_log_message);
    } else {
        print_hex_dump(&log_message->u.hex_dump_message);
    }
}

#if defined(MTK_PORT_SERVICE_ENABLE)

static void syslog_serial_port_open(TimerHandle_t timer)
{
    serial_port_open_para_t serial_port_open_para;
    serial_port_status_t status;

    portENTER_CRITICAL();
    if (g_log_port == (serial_port_dev_t)g_log_uart_port) {
        hal_uart_deinit(g_log_uart_port);
    }
    serial_port_open_para.callback = syslog_port_service_callback;
    status = serial_port_open(g_log_port, &serial_port_open_para, &g_log_port_handle);
    portEXIT_CRITICAL();

    switch (status) {
        case SERIAL_PORT_STATUS_DEV_NOT_READY:
            break;

        case SERIAL_PORT_STATUS_OK:
            /* port service ready */
            log_switch_to_dma_mode();
            break;

        case SERIAL_PORT_STATUS_BUSY:
        case SERIAL_PORT_STATUS_UNINITIALIZED:
        case SERIAL_PORT_STATUS_UNSUPPORTED:
        case SERIAL_PORT_STATUS_INVALID_DEVICE:
        case SERIAL_PORT_STATUS_INVALID_PARAMETER:
        case SERIAL_PORT_STATUS_FAIL:
            /* not recoverable */
            break;
    }

    LOG_I(common, "open port status = %d", status);
}

void syslog_serial_port_switch(serial_port_dev_t device)
{
    syslog_init_done &= ~(SYSLOG_UART_DMA_MODE | SYSLOG_PORT_OPEN_DONE);
    serial_port_close(g_log_port_handle);
    if (g_log_port == (serial_port_dev_t)g_log_uart_port) {
        log_uart_init(g_log_uart_port);
    }
    g_log_port = device;
    syslog_serial_port_open(NULL);
}

#endif /* MTK_PORT_SERVICE_ENABLE */

void syslog_task_entry(void *args)
{
#if defined(MTK_PORT_SERVICE_ENABLE)
    tx_sem = xSemaphoreCreateBinary();
    rx_sem = xSemaphoreCreateBinary();
    /* The port setting is written during project initialization. Syslog reads the port setting. */
    if (serial_port_config_read_dev_number("syslog", &g_log_port) != SERIAL_PORT_STATUS_OK) {
        g_log_port = (serial_port_dev_t)g_log_uart_port;
    }
    LOG_I(common, "syslog port = %d", g_log_port);
    syslog_serial_port_open(NULL);
#endif

    syslog_task_ready = true;
    vTaskPrioritySet(NULL, TASK_PRIORITY_SYSLOG);

    for (;;) {
        syslog_buffer_t *log_buffer = NULL;
        if (0 == log_queue_receive(&log_buffer)) {
#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)
if(syslog_switch_from_uart_to_flash() == true) {
            process_log_message_variant((log_message_t *)(log_buffer->buffer));
} else {
            process_log_message((log_message_t *)(log_buffer->buffer));
}

#else
   process_log_message((log_message_t *)(log_buffer->buffer));
#endif
            buffer_pool_release(log_buffer);
        }
    }
}

void log_init(syslog_save_fn         save,
              syslog_load_fn         load,
              log_control_block_t  **entries)
{
    exception_config_type exception_config;

    if (syslog_init_done & SYSLOG_GPT_INIT_DONE) {
        /* avoid initializing syslog twice */
        return;
    }

    syslog_init_done |= SYSLOG_UART_INIT_DONE; /* for backward compatible */
    buffer_pool_init();
    log_queue_create();
    syslog_task_ready = false;

    exception_config.init_cb = syslog_exception_cb;
    exception_config.dump_cb = NULL;
    exception_register_callbacks(&exception_config);


    if (pdPASS != xTaskCreate(syslog_task_entry,
                              SYSLOG_TASK_NAME,
                              SYSLOG_TASK_STACKSIZE,
                              NULL,
                              TASK_PRIORITY_HARD_REALTIME,
                              &syslog_task_handle)) {
        printf("log_init: create syslog task failed.\r\n");
    }

    syslog_init_time = get_current_time_in_ms();
    syslog_init_done |= SYSLOG_GPT_INIT_DONE;
    (void)syslog_init_time;

    syslog_config.save_fn = save;
    syslog_config.filters = entries;

    if (load) {
        /* overwrite the rom code definition if any */
        if (load(&syslog_config) != 0) {
            LOG_E(common, "load syslog config failed\n");
        }
    }
    syslog_init_done |= SYSLOG_NVDM_INIT_DONE;
}

void vprint_module_log(void *handle,
                       const char *func,
                       int line,
                       print_level_t level,
                       const char *message,
                       va_list list)
{
    log_control_block_t *context = (log_control_block_t *)handle;
    if ((context->log_switch == DEBUG_LOG_ON)
            && (context->print_level <= level)) {

        if (!syslog_task_ready
                || (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED
                    && (portNVIC_INT_CTRL_REG & 0xff) == 0)) {
            if (syslog_init_done > 0) {
                UBaseType_t uxSavedInterruptStatus;
                uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
                if (syslog_init_done > SYSLOG_UART_INIT_DONE) {
                    /* gpt init done, time can be printed  */
                    printf("[T: %u M: %s C: %s F: %s L: %d]: ",
                           get_current_time_in_ms(),
                           context->module_name,
                           change_level_to_string(level),
                           func,
                           line);
                }
                vprintf(message, list);
                printf("\r\n");
                portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );
            }
        } else {
            /* syslog task is ready, send to queue to print */
            syslog_buffer_t *syslog_buffer = buffer_pool_alloc();
            log_message_t *log_message;
            normal_log_message_t *normal_log_message;

            if (NULL == syslog_buffer) {
                return;
            }
            log_message = (log_message_t *)syslog_buffer->buffer;
            log_message->message_type = MESSAGE_TYPE_NORMAL_LOG;
            normal_log_message = &log_message->u.normal_log_message;
            normal_log_message->module_name = context->module_name;
            normal_log_message->print_level = level;
            normal_log_message->func_name = func;
            normal_log_message->line_number = line;
            normal_log_message->timestamp = get_current_time_in_ms();

            /* This step might fail when log message is too long,
               but syslog will do it's best to print the log */
            (void)vsnprintf(normal_log_message->message, MAX_LOG_SIZE, message, list);
            log_queue_send(&syslog_buffer);
        }
    }
}

void print_module_log(void *handle,
                      const char *func,
                      int line,
                      print_level_t level,
                      const char *message, ...)
{
    va_list ap;
    va_start(ap, message);
    vprint_module_log(handle, func, line, level, message, ap);
    va_end(ap);
}

void vdump_module_buffer(void *handle,
                         const char *func,
                         int line,
                         print_level_t level,
                         const void *data,
                         int length,
                         const char *message,
                         va_list list)
{
    log_control_block_t *context = (log_control_block_t *)handle;
    int additional_length;

    if ((context->log_switch == DEBUG_LOG_ON)
            && (context->print_level <= level)) {

        if (!syslog_task_ready
                || (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED
                    && (portNVIC_INT_CTRL_REG & 0xff) == 0)) {
            if (syslog_init_done > 0) {
                UBaseType_t uxSavedInterruptStatus;
                uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
                if (syslog_init_done > SYSLOG_UART_INIT_DONE) {
                    /* gpt init done, time can be printed  */
                    printf("[T: %u M: %s C: %s F: %s L: %d]: ",
                           get_current_time_in_ms(),
                           context->module_name,
                           change_level_to_string(level),
                           func,
                           line);
                }
                vprintf(message, list);
                printf("\r\n");
                hex_dump_data((char *)data, length);
                portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );
            }
        } else {
            /* syslog task is ready, send to queue to print */
            syslog_buffer_t *syslog_buffer = buffer_pool_alloc();
            log_message_t *log_message;
            hex_dump_message_t *hex_dump_message;

            if (NULL == syslog_buffer) {
                return;
            }

            log_message = (log_message_t *)syslog_buffer->buffer;
            log_message->message_type = MESSAGE_TYPE_HEX_DUMP;
            hex_dump_message = &log_message->u.hex_dump_message;
            hex_dump_message->module_name = context->module_name;
            hex_dump_message->print_level = level;
            hex_dump_message->func_name = func;
            hex_dump_message->line_number = line;
            hex_dump_message->timestamp = get_current_time_in_ms();

            /* This step might fail when log message is too long,
               but syslog will do it's best to print the log */
            (void)vsnprintf(hex_dump_message->message, MAX_DUMP_MESSAGE_SIZE, message, list);

            if (length > MAX_HEXDUMP_LENGTH) {
                length = MAX_HEXDUMP_LENGTH;
            }

            additional_length = 0;
            if (length > MAX_DUMP_DATA_SIZE) {
                if ( (portNVIC_INT_CTRL_REG & 0xff) == 0) {
                    additional_length = length - MAX_DUMP_DATA_SIZE;
                }
                length = MAX_DUMP_DATA_SIZE;
            }

            memcpy(hex_dump_message->data, data, length);
            hex_dump_message->data_length = length;

            hex_dump_message->additional_data = NULL;
            if (additional_length) {
                if ((hex_dump_message->additional_data = (char *) pvPortMalloc(additional_length)) != NULL) {
                    memcpy(hex_dump_message->additional_data, (char *)data + length, additional_length);
                    hex_dump_message->data_length += additional_length;
                }
            }

            log_queue_send(&syslog_buffer);
        }
    }
}

void dump_module_buffer(void *handle,
                        const char *func,
                        int line,
                        print_level_t level,
                        const void *data,
                        int length,
                        const char *message, ...)
{
    va_list ap;
    va_start(ap, message);
    vdump_module_buffer(handle, func, line, level, data, length, message, ap);
    va_end(ap);
}


void syslog_get_config(syslog_config_t *config)
{
    *config = syslog_config;
}

/**
 * @param p  the string pointer.
 * @param pp the pointer be assigned the start of next string.
 *
 * @retval 0 No valid string found.
 * @return the length of the found string.
 */
static int syslog_get_a_string(const char *p, char **pp)
{
    const char *start;

    while (*p == ' ') {
        p++;
    }

    start = p;

    /* first non-space is '\0' */
    if (*p == '\0') {
        *pp = NULL;
        return 0;
    }

    *pp = (char *)p;

    while (*p != ' ' && *p != '\0') {
        p++;
    }

    return (p - start);
}

static int syslog_get_index_from_module_name(log_control_block_t **filters, const char *module_name)
{
    int i = 0;

    if (filters == NULL) {
        return -1;
    }

    while (filters[i]) {
        if (!strcmp(filters[i]->module_name, module_name)) {
            return i;
        }
        i++;
    }

    /* no match tag name found */
    return -1;
}

int syslog_set_filter(char *module_name, char *log_switch_str, char *print_level_str, int save)
{
    int         i;
    int         log_switch;
    int         print_level;

    log_switch = str_to_log_switch(log_switch_str);
    if (log_switch < 0) {
        return -1;
    }

    print_level = str_to_print_level(print_level_str);
    if (print_level < 0) {
        return -1;
    }

    if (strcmp(module_name, "*") == 0) {

        if (syslog_config.filters == NULL) {
            return -1;
        }

        i = 0;
        while (syslog_config.filters[i]) {
            syslog_config.filters[i]->log_switch = (log_switch_t)log_switch;
            syslog_config.filters[i]->print_level = (print_level_t)print_level;
            i++;
        }

    } else {

        i = syslog_get_index_from_module_name(syslog_config.filters, module_name);
        /* no match tag name found */
        if (i < 0) {
            return -1;
        }
        syslog_config.filters[i]->log_switch = (log_switch_t)log_switch;
        syslog_config.filters[i]->print_level = (print_level_t)print_level;

    }

    if (save) {
        syslog_config.save_fn(&syslog_config);
    }

    return 0;
}

int syslog_at_set_filter(char *module_name, int log_switch, int print_level, int save)
{
    int         i;

    if (strcmp(module_name, "*") == 0) {

        if (syslog_config.filters == NULL) {
            return -1;
        }

        i = 0;
        while (syslog_config.filters[i]) {
            syslog_config.filters[i]->log_switch = (log_switch_t)log_switch;
            syslog_config.filters[i]->print_level = (print_level_t)print_level;
            i++;
        }

    } else {

        i = syslog_get_index_from_module_name(syslog_config.filters, module_name);
        /* no match tag name found */
        if (i < 0) {
            return -1;
        }
        syslog_config.filters[i]->log_switch = (log_switch_t)log_switch;
        syslog_config.filters[i]->print_level = (print_level_t)print_level;

    }

    if (save) {
        syslog_config.save_fn(&syslog_config);
    }

    return 0;
}

/**
 * Find name and filter spec pair and convert into filter.
 */
int syslog_convert_filter_str2val(log_control_block_t **filters, char *buff)
{
    int     pos = 0;
    int     i;
    char    *module_name;
    char    *log_switch_str;
    char    *print_level_str;
    bool    done = false;
    uint32_t    log_switch;
    uint32_t    print_level;
    uint8_t     type;

    while (!done) {

        /*
         * 1. find module_name
         */

        i = syslog_get_a_string(&buff[pos], &module_name);
        if (i == 0 || buff[pos + i] == '\0') {
            /* premature end */
            return -1;
        }

        buff[pos + i]  = '\0';
        pos           += i + 1;

        /*
         * 2. find log_switch
         */

        i = syslog_get_a_string(&buff[pos], &log_switch_str);
        if (i == 0) {
            /* premature end */
            return -2;
        }

        done = (buff[i + pos] == '\0');

        buff[pos + i]  = '\0';
        pos           += i + 1;

        /*
         * 3. find print_level
         */

        i = syslog_get_a_string(&buff[pos], &print_level_str);
        if (i == 0) {
            /* premature end */
            return -2;
        }

        done = (buff[i + pos] == '\0');

        buff[pos + i]  = '\0';
        pos           += i + 1;

        /* 4. check and set */

        i = syslog_get_index_from_module_name(filters, module_name);

        /* no match tag found */
        if (i < 0) {
            return -3;
        }

        log_switch = toi(log_switch_str, &type);

        /* not a valid number */
        if (type == TOI_ERR) {
            return -4;
        }

        /* handle out of range */
        log_switch = (log_switch > DEBUG_LOG_OFF) ? DEBUG_LOG_OFF : log_switch;

        print_level = toi(print_level_str, &type);

        /* not a valid number */
        if (type == TOI_ERR) {
            return -4;
        }

        /* handle out of range */
        print_level = (print_level > PRINT_LEVEL_ERROR) ? PRINT_LEVEL_ERROR : print_level;

        filters[i]->log_switch = (log_switch_t)log_switch;
        filters[i]->print_level = (print_level_t)print_level;
    }

    return 0;
}

int syslog_convert_filter_val2str(const log_control_block_t **filters, char *buff)
{
    int i   = 0;
    int pos = 0;
    int n;

    while (filters && filters[i]) {
        n = snprintf(&buff[pos], SYSLOG_FILTER_LEN - pos, "%s%s 0x%01x 0x%01x",
                    (i == 0) ? "" : " ",
                    filters[i]->module_name,
                    filters[i]->log_switch,
                    filters[i]->print_level);
        pos += n;
        i++;
    }

    configASSERT (pos < SYSLOG_FILTER_LEN);

    return pos;
}

const char *log_switch_to_str(log_switch_t log_switch)
{
    configASSERT (log_switch - DEBUG_LOG_ON <= DEBUG_LOG_OFF);
    return  log_switch_table[log_switch];
}

const char *print_level_to_str(print_level_t print_level)
{
    configASSERT (print_level - PRINT_LEVEL_DEBUG <= PRINT_LEVEL_ERROR);
    return  print_level_table[print_level];
}

int str_to_log_switch(const char *log_switch_str)
{
    int i;

    for (i = 0; i < sizeof(log_switch_table) / sizeof(char *); i++) {
        if (!strcmp(log_switch_str, log_switch_table[i])) {
            return i;
        }
    }
    return -1;
}

int str_to_print_level(const char *print_level_str)
{
    int i;

    for (i = 0; i < sizeof(print_level_table) / sizeof(char *); i++) {
        if (!strcmp(print_level_str, print_level_table[i])) {
            return i;
        }
    }
    return -1;
}

int __wrap_printf( const char *format, ... )
{
    va_list ap;
    int ret;
    UBaseType_t uxSavedInterruptStatus;

    if ( !syslog_init_done ) {
        return 0;
    }

    uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();

    va_start(ap, format);
    ret = vprintf(format, ap);
    va_end(ap);

    portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );

    return ret;
}

#else

#include "syslog.h"

#if defined(MTK_PORT_SERVICE_ENABLE)
#include "serial_port.h"
#endif

/* create common module for unspecified module to use */
log_create_module(common, PRINT_LEVEL_INFO);

static hal_uart_port_t g_log_uart_port;

hal_uart_status_t log_uart_init(hal_uart_port_t port)
{
    hal_uart_config_t uart_config;
    hal_uart_status_t ret;

    /* Configure UART PORT */
    uart_config.baudrate = HAL_UART_BAUDRATE_115200;
    uart_config.parity = HAL_UART_PARITY_NONE;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;

    g_log_uart_port = port;
    ret = hal_uart_init(port, &uart_config);

    return ret;
}

void log_putchar(char byte)
{
    hal_uart_put_char(g_log_uart_port, byte);
}

void print_module_log(void *handle,
                      const char *func,
                      int line,
                      print_level_t level,
                      const char *message, ...)
{
}

void dump_module_buffer(void *handle,
                        const char *func,
                        int line,
                        print_level_t level,
                        const void *data,
                        int length,
                        const char *message, ...)
{
}

void vprint_module_log(void *handle,
                       const char *func,
                       int line,
                       print_level_t level,
                       const char *message,
                       va_list list)
{
}

void vdump_module_buffer(void *handle,
                         const char *func,
                         int line,
                         print_level_t level,
                         const void *data,
                         int length,
                         const char *message,
                         va_list list)
{
}

#if defined (MTK_PORT_SERVICE_ENABLE)

int log_write(char *buf, int len)
{
    return 0;
}

void syslog_serial_port_switch(serial_port_dev_t device)
{
}

#endif /* MTK_PORT_SERVICE_ENABLE */

#endif /* MTK_DEBUG_LEVEL_NONE */
