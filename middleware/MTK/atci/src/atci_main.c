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

/* System Header file */
#include "FreeRTOS.h"
#include "task.h"
#include "hal_uart.h"
#include "syslog.h"
#include "atci_adapter.h"
#include "atci_main.h"

#include "at_command.h"
#include "memory_attribute.h"
#include <string.h>
#include <stdio.h>

log_create_module(atci, PRINT_LEVEL_WARNING);
#define LOGE(fmt,arg...)   LOG_E(atci, "ATCI main: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atci, "ATCI main: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atci ,"ATCI main: "fmt,##arg)

#define _unused(x) ((void)x)

/* ATCI main body related */
// g_atci_local_startup_flag = 0 means atci deinit; 1 means atci can work normal; 2 means in mode switching; 3 means atci just do local init.
uint8_t  g_atci_local_startup_flag = 0;
uint32_t g_atci_handler_mutex = NULL;
uint32_t g_atci_handler_semaphore = NULL;
uint32_t g_atci_processing_semaphore = NULL;

uint32_t g_atci_input_command_queue = NULL;
uint32_t g_atci_resonse_command_queue = NULL;
uint32_t g_atci_data_cached_queue = NULL;
uint32_t g_atci_registered_table_number = 0;
uint32_t atci_input_commad_in_processing = 0;

atci_cmd_hdlr_table_t g_atci_cm4_general_hdlr_tables[ATCI_MAX_GNENERAL_TABLE_NUM] = {{0}};

static atci_send_input_cmd_msg_t   g_input_command = {{0}};

#if defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)

serial_port_dev_t g_serial_port = SERIAL_PORT_DEV_UART_1;
serial_port_dev_t g_switched_serial_port = SERIAL_PORT_DEV_UART_1;
//serial_port_handle_t g_serial_port_handle = 0;
serial_port_handle_t switched_serial_port_handle = 0;

#else
hal_uart_port_t g_atci_uart_port = HAL_UART_0;
typedef struct
{
    char    uart_rx_buff[ATCI_UART_RX_FIFO_BUFFER_SIZE];
    char    uart_tx_buff[ATCI_UART_TX_FIFO_BUFFER_SIZE];
} atci_uart_cntx_st;


ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static atci_uart_cntx_st  g_atci_uart_cntx;

#endif

uint32_t g_atci_port_handle = 0;

// for send data cached
uint32_t g_send_data_cache_left_len = 0;
uint32_t g_send_data_cache_len = 0;
uint8_t* g_send_data_cache = NULL;

atci_status_t atci_init_int(void);
atci_status_t atci_read_data(void);
atci_status_t atci_port_deinit(uint32_t port);
atci_status_t atci_data_processing_for_deinit(void);
atci_status_t atci_vaild_port_check(uint32_t port);


#if defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)

void atci_serial_port_data_callback(serial_port_dev_t device, serial_port_callback_event_t event, void *parameter)
{
    atci_general_msg_t msg_queue_item;
	 //uint16_t		   queue_msg_num = 0;

    //LOGW("atci_serial_port_data_callback, event = %d, atci status = %d\r\n", event, g_atci_local_startup_flag);

    switch(event)
    {
        case SERIAL_PORT_EVENT_READY_TO_READ:
        {
            LOGI("atci_serial_port_data_callback, read event\r\n");
			
			if((g_atci_local_startup_flag == 3) && 
				((g_serial_port == SERIAL_PORT_DEV_USB_COM1) || (g_serial_port == SERIAL_PORT_DEV_USB_COM2)))
			{
                atci_init_int();
			}
			
            if (atci_check_startup_finish())
            {
                LOGI("atci_serial_port_data_callback 1, read event\r\n");
				//queue_msg_num = atci_queue_get_item_num(g_atci_input_command_queue);

				//if (queue_msg_num >= ATCI_LOCAL_QUEUE_LENGTH)
				//	LOGW("atci_serial_port_data_callback, input read queue full (len: %d)\r\n", queue_msg_num);
				
				msg_queue_item.msg_id = MSG_ID_ATCI_LOCAL_SEND_CMD_IND;
				msg_queue_item.msg_data = NULL;
				atci_queue_send(g_atci_input_command_queue, (void*)&msg_queue_item);
				atci_semaphore_give(g_atci_handler_semaphore);
            }

            break;
        }

        case SERIAL_PORT_EVENT_READY_TO_WRITE:
        {
            if (atci_check_startup_finish() && g_send_data_cache_left_len)
            {
            	//queue_msg_num = atci_queue_get_item_num(g_atci_data_cached_queue);

				//if (queue_msg_num >= ATCI_LOCAL_QUEUE_LENGTH)
				//	LOGW("atci_serial_port_data_callback, cache data queue full (len: %d)\r\n", queue_msg_num);

                LOGI("atci_serial_port_data_callback, write event\r\n");
                msg_queue_item.msg_id = MSG_ID_ATCI_LOCAL_WRITE_CMD_IND;

                atci_queue_send(g_atci_data_cached_queue, (void*)&msg_queue_item);
                atci_semaphore_give(g_atci_handler_semaphore);
            }
            break;
        }
#if 0
        case SERIAL_PORT_EVENT_BTSPP_CONNECTION:
        {
            LOGW("atci_serial_port_data_callback, btspp connection event(parameter:%d)\r\n", parameter);

            if(parameter == BT_STATUS_SUCCESS)
            {
                // connect success
                if(g_atci_local_startup_flag == 2)
                {
                    // close serial port
                    serial_port_close(g_atci_port_handle);
                    g_atci_port_handle = switched_serial_port_handle;
                    g_atci_local_startup_flag = 1;
                    g_serial_port = g_switched_serial_port;
                    LOGW("atci_serial_port_data_callback() switch to BTSPP success\r\n");
                    return;
                }
                else if(g_atci_local_startup_flag == 3)
                    atci_init_int();
            }
            else
            {
                // connect fail
                if(g_atci_local_startup_flag == 2)
                {
                    // keep old serial port
                    switched_serial_port_handle = 0;
                    g_atci_local_startup_flag = 1;
                    g_switched_serial_port = SERIAL_PORT_DEV_UART_TYPE_BEGIN;
                    LOGW("atci_serial_port_data_callback() switch to BTSPP fail\r\n");
                    return;
                }
                else if(g_atci_local_startup_flag == 3)
                    serial_port_close(g_atci_port_handle);
            }

            break;
        }

        case SERIAL_PORT_EVENT_BTSPP_DISCONNECTION:
        {
            LOGW("atci_serial_port_data_callback, btspp disconnection event (atci status:%d)\r\n", g_atci_local_startup_flag);

            if (g_atci_local_startup_flag == 1)
                atci_deinit((hal_uart_port_t)g_atci_port_handle);
            else if(g_atci_local_startup_flag == 2)
            {
                g_atci_local_startup_flag = 1;
                g_serial_port = g_switched_serial_port;
            }
            break;
        }
#endif
        case SERIAL_PORT_EVENT_USB_CONNECTION:
        {
            LOGI("atci_serial_port_data_callback, usb connection event\r\n");

            if(g_atci_local_startup_flag == 2)
            {
                // close serial port
                serial_port_close(g_atci_port_handle);
                g_atci_port_handle = switched_serial_port_handle;
                g_atci_local_startup_flag = 1;
                g_serial_port = g_switched_serial_port;
                LOGI("atci_serial_port_data_callback() switch to usb success\r\n"); 
                return;
            }
            else if(g_atci_local_startup_flag == 3)
                atci_init_int();

            break;
        }
        case SERIAL_PORT_EVENT_USB_DISCONNECTION:
        {
            LOGI("atci_serial_port_data_callback, usb disconnection event\r\n");
            //atci_deinit_ext();
            break;
        }

        default:
            break;
   }

}

atci_status_t atci_serial_port_init(serial_port_dev_t port)
{
    atci_status_t ret = ATCI_STATUS_OK;
    serial_port_status_t status;

    serial_port_open_para_t serial_port_open_para;

    LOGI("atci_serial_port_init() enter,select port:%d \r\n", port);

    if (g_atci_local_startup_flag == 1 || g_atci_local_startup_flag == 2)
    {
        LOGW("atci_serial_port_init() already inited \r\n");
        return ATCI_STATUS_OK;
    }

    g_serial_port = port;

    // register the serial user event callback
    serial_port_open_para.callback = atci_serial_port_data_callback;
 
    // for serial port configuration
    status = serial_port_open(port, &serial_port_open_para, &g_atci_port_handle);
    LOGW("atci_serial_port_init() serial_port_open ret=%d \r\n", status);

    switch (status) {
        case SERIAL_PORT_STATUS_DEV_NOT_READY:
            /* for usb */
            break;

        case SERIAL_PORT_STATUS_OK:
            /* port service ready */
            ret = atci_init_int();
            break;

 //       case SERIAL_PORT_STATUS_PORT_PENDING: /* for bt spp */
            /* wait for bp-spp connection ready before using it */
 //           break;

        case SERIAL_PORT_STATUS_BUSY:
        case SERIAL_PORT_STATUS_UNINITIALIZED:
        case SERIAL_PORT_STATUS_UNSUPPORTED:
        case SERIAL_PORT_STATUS_INVALID_DEVICE:
        case SERIAL_PORT_STATUS_INVALID_PARAMETER:
        case SERIAL_PORT_STATUS_FAIL:
        default :
			/* not recoverable */
			ret = ATCI_STATUS_ERROR;
            break;
    }

	return ret;
}

static atci_status_t atci_serial_port_deinit(serial_port_handle_t handle)
{
    atci_status_t ret = ATCI_STATUS_ERROR;
    serial_port_status_t status;

    status = serial_port_close(handle);
    if (status != SERIAL_PORT_STATUS_OK)
    {
        LOGE("atci_serial_port_deinit() fail \r\n");
        ret = ATCI_STATUS_ERROR;
    }
    else
    {
        ret = ATCI_STATUS_OK;
        LOGI("atci_serial_port_deinit() success \r\n");
    }

    return ret;
}

uint32_t atci_serial_port_read_data(serial_port_handle_t handle, uint8_t *buf, uint32_t buf_len)
{
    serial_port_read_data_t read_data;
    serial_port_status_t status = SERIAL_PORT_STATUS_FAIL;
    uint32_t data_len = 0;

	read_data.buffer = (uint8_t*)buf;
    read_data.size = buf_len;

    status = serial_port_control(g_atci_port_handle, SERIAL_PORT_CMD_READ_DATA, (serial_port_ctrl_para_t*)&read_data);
    if (status != SERIAL_PORT_STATUS_OK)
	{
        LOGE("atci_serial_port_read_data() serial_port_control read fail \r\n");
        return data_len;
    }

    data_len = read_data.ret_size;

    return data_len;
}

uint32_t atci_serial_port_send_data(uint32_t port, uint8_t* buf, uint32_t buf_len)
{
    serial_port_write_data_t send_data;
    uint32_t data_len = 0;
    serial_port_status_t status = SERIAL_PORT_STATUS_OK;

    if(port == 0)
        return data_len;

    send_data.data = buf;
    send_data.size = buf_len;

    status = serial_port_control(port, SERIAL_PORT_CMD_WRITE_DATA, (serial_port_ctrl_para_t*)&send_data);
    data_len = send_data.ret_size;

    if (status != SERIAL_PORT_STATUS_OK)
    {
        LOGE("atci_serial_port_send_data() send data fail\r\n");
    }

    return data_len;

}

void atci_serial_port_switch(serial_port_dev_t device)
{
    LOGI("atci_serial_port_switch() enter switch port:%d\r\n", device);

    serial_port_status_t status;
    serial_port_open_para_t serial_port_open_para;

    if(device == g_serial_port)
    {
        LOGW("atci_serial_port_switch() switch port == current port \r\n");
        return;
    }

    g_switched_serial_port = device;

    g_atci_local_startup_flag = 2;

    atci_data_processing_for_deinit();

    // register the serial user event callback
    serial_port_open_para.callback = atci_serial_port_data_callback;

    // for serial port configuration
    status = serial_port_open(device, &serial_port_open_para, &switched_serial_port_handle);
    LOGI("atci_serial_port_switch() serial_port_open ret=%d \r\n", status);

    switch (status) {
        case SERIAL_PORT_STATUS_DEV_NOT_READY:
            break;

        case SERIAL_PORT_STATUS_OK:
            // close serial port
            status = serial_port_close(g_atci_port_handle);
            g_atci_port_handle = switched_serial_port_handle;
		#if 0
            if((g_serial_port >= SERIAL_PORT_DEV_BTSPP_SERVER1) && (g_serial_port <= SERIAL_PORT_DEV_BTSPP_CLIENT2))
                g_atci_local_startup_flag = 2;
            else
		#endif
            {
                g_serial_port = g_switched_serial_port;
                g_atci_local_startup_flag = 1;
            }
            LOGI("atci_serial_port_switch() switch port success atci_status:%d \r\n", g_atci_local_startup_flag);
            break;

 //       case SERIAL_PORT_STATUS_PORT_PENDING: /* for bt spp */
            /* wait for bp-spp connection ready before using it */
  //          break;

        case SERIAL_PORT_STATUS_BUSY:
        case SERIAL_PORT_STATUS_UNINITIALIZED:
        case SERIAL_PORT_STATUS_UNSUPPORTED:
        case SERIAL_PORT_STATUS_INVALID_DEVICE:
        case SERIAL_PORT_STATUS_INVALID_PARAMETER:
        case SERIAL_PORT_STATUS_FAIL:
        default :
            /* not recoverable */
            g_atci_local_startup_flag = 1;
            LOGW("atci_serial_port_switch() end, switch fail because the port is busy \r\n");
            break;
    }

    LOGI("atci_serial_port_switch() end\r\n");
}

atci_status_t atci_init_ext(serial_port_dev_t port)
{
    atci_status_t ret = ATCI_STATUS_ERROR;
    ret = atci_serial_port_init(port);
    return ret;
}

#else

void atci_uart_irq(hal_uart_callback_event_t event, void *parameter)
{
    atci_general_msg_t msg_queue_item;

    //LOGW("atci_uart_irq, hal_uart_cb_event: %d \r\n", event);

    if (g_atci_port_handle >= HAL_UART_MAX){

        LOGW("atci_uart_irq, invaild uart port:%d\r\n", g_atci_port_handle);
        return;
    }

    if (HAL_UART_EVENT_READY_TO_READ == event)
    {
        LOGI("atci_uart_irq, read event\r\n");
        if (atci_check_startup_finish())
        {
            msg_queue_item.msg_id = MSG_ID_ATCI_LOCAL_SEND_CMD_IND;
            msg_queue_item.msg_data = NULL;
            atci_queue_send(g_atci_input_command_queue, (void*)&msg_queue_item);
            atci_semaphore_give(g_atci_handler_semaphore);
        }
    }
    else if (HAL_UART_EVENT_READY_TO_WRITE == event) {
        if (atci_check_startup_finish() && g_send_data_cache_left_len)
        {
            LOGI("atci_uart_irq, write event\r\n");
            msg_queue_item.msg_id = MSG_ID_ATCI_LOCAL_WRITE_CMD_IND;

            atci_queue_send(g_atci_data_cached_queue, (void*)&msg_queue_item);
            atci_semaphore_give(g_atci_handler_semaphore);
        }
    }
}

static atci_status_t atci_uart_init(hal_uart_port_t port)
{
    hal_uart_config_t uart_config;
    hal_uart_dma_config_t dma_config;
    atci_status_t ret = ATCI_STATUS_ERROR;

    LOGI("atci_uart_init,port=%d \r\n", port);
	if(port >= HAL_UART_MAX)
        return ATCI_STATUS_ERROR;

    /* Configure UART PORT */
    uart_config.baudrate = HAL_UART_BAUDRATE_115200;
    uart_config.parity = HAL_UART_PARITY_NONE;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;

    g_atci_port_handle = port;

    if (HAL_UART_STATUS_OK != hal_uart_init(port, &uart_config)) {
        // log: uart init fail
        LOGE("hal_uart_init fail\r\n");
        ret = ATCI_STATUS_ERROR;
    } else {
        // log: uart init success
        LOGI("hal_uart_init success \n");
        ret = ATCI_STATUS_OK;
        dma_config.receive_vfifo_alert_size = ATCI_UART_RX_FIFO_ALERT_SIZE;
        dma_config.receive_vfifo_buffer = (uint8_t *)g_atci_uart_cntx.uart_rx_buff;
        dma_config.receive_vfifo_buffer_size = ATCI_UART_RX_FIFO_BUFFER_SIZE;
        dma_config.receive_vfifo_threshold_size = ATCI_UART_RX_FIFO_THRESHOLD_SIZE;
        dma_config.send_vfifo_buffer = (uint8_t *)g_atci_uart_cntx.uart_tx_buff;
        dma_config.send_vfifo_buffer_size = ATCI_UART_TX_FIFO_BUFFER_SIZE;
        dma_config.send_vfifo_threshold_size = ATCI_UART_TX_FIFO_THRESHOLD_SIZE;

        hal_uart_set_dma(port, &dma_config);
        hal_uart_register_callback(port, atci_uart_irq, NULL);
    }

    return ret;
}

static atci_status_t atci_uart_deinit(hal_uart_port_t port)
{
    hal_uart_status_t uart_ret = HAL_UART_STATUS_ERROR;
    atci_status_t ret = ATCI_STATUS_ERROR;

    uart_ret = hal_uart_deinit(port);

    LOGI("atci_uart_deinit() uart_ret = %d \r\n", uart_ret);

    if (uart_ret == HAL_UART_STATUS_OK) {

        ret = ATCI_STATUS_OK;
    }

    return ret;
}

static uint32_t atci_uart_read_data(hal_uart_port_t uart_index, uint8_t *buf, uint32_t buf_len)
{
#define EARLY_ACCESS_7687EVB_TEST
#ifdef EARLY_ACCESS_7687EVB_TEST
    uint32_t available_data_len = 0;
    available_data_len = hal_uart_get_available_receive_bytes(uart_index);
    if (available_data_len > buf_len) {
        available_data_len = buf_len;
    }
    hal_uart_receive_dma(uart_index, (uint8_t *)buf, available_data_len);
    return available_data_len;
#else
    return hal_uart_receive_dma(uart_index, (uint8_t *)buf, (uint32_t) buf_len);
#endif
}

uint32_t atci_uart_send_data(hal_uart_port_t port, uint8_t *buf, uint32_t buf_len)
{
    uint32_t ret_len = 0;

	ret_len = hal_uart_send_dma(port, buf, buf_len);
    return ret_len;
}

#endif

uint8_t atci_check_startup_finish(void)
{
    uint8_t is_finish = 0;
	if(g_atci_local_startup_flag == 1)
	{
		is_finish = 1;
	}
     return is_finish;
}


/* ATCI main body related */
atci_status_t atci_local_init(void)
{
    atci_status_t ret =  ATCI_STATUS_ERROR;
    /* Init Mutex */

    LOGI("atci_local_init() enter flag:%d \r\n", g_atci_local_startup_flag);

    if (g_atci_local_startup_flag != 0) {

        return ATCI_STATUS_OK;
    }

/*	if (NULL == (void *)g_atci_handler_mutex) {
		
		g_atci_handler_mutex = atci_mutex_create();
		if (NULL == (void *)g_atci_handler_mutex) {
			return ret;
		}
	}
*/
    if (NULL == (void *)g_atci_handler_semaphore) {

    g_atci_handler_semaphore = atci_semaphore_create(10, 0);
        if (NULL == (void *)g_atci_handler_semaphore) {
            return ret;
        }
    }

/*    g_atci_registered_table_number = 0;
    for ( i = 0; i < ATCI_MAX_GNENERAL_TABLE_NUM; i++) {
        g_atci_cm4_general_hdlr_tables[i].item_table = NULL;
        g_atci_cm4_general_hdlr_tables[i].item_table_size = 0;
    }
*/
    /* Init Queue */
    if (NULL == (void *)g_atci_input_command_queue) {
        g_atci_input_command_queue = atci_queue_create(ATCI_LOCAL_QUEUE_LENGTH,sizeof(atci_general_msg_t));
        if (NULL == (void *)g_atci_input_command_queue) {
            return ret;
        }
    }

    if (NULL == (void *)g_atci_resonse_command_queue) {
         g_atci_resonse_command_queue = atci_queue_create(ATCI_LOCAL_QUEUE_LENGTH,sizeof(atci_general_msg_t));
        if (NULL == (void *)g_atci_resonse_command_queue) {
            return ret;
        }
    }

    if (NULL == (void *)g_atci_data_cached_queue) {
        g_atci_data_cached_queue = atci_queue_create(ATCI_LOCAL_QUEUE_LENGTH,sizeof(atci_general_msg_t));
        if (NULL == (void *)g_atci_data_cached_queue) {
            return ret;
        }
    }

    atci_input_commad_in_processing = 0;

    g_atci_local_startup_flag = 3;

    if ( NULL != (void *)g_atci_processing_semaphore )
        atci_semaphore_give(g_atci_processing_semaphore);

    ret =  ATCI_STATUS_OK;
    LOGI("atci_local_init() success \r\n");
    return ret;
}

atci_status_t atci_init_int(void)
{
    atci_status_t ret = ATCI_STATUS_OK;

    LOGI("atci_init_int(), enter\r\n");

  //  ret = atci_local_init();

   // if (ret != ATCI_STATUS_OK) {
   //     ret = atci_port_deinit(g_atci_port_handle);
   //     return ret;
  //  }

    if (at_command_init() == ATCI_STATUS_OK) {

        LOGI("atci_init_int() success \r\n");
    } else {

        ret = ATCI_STATUS_ERROR;
        LOGE("atci_init_int() fail \r\n");
    }
    g_atci_local_startup_flag = 1;
    return ret;
}

atci_status_t atci_init(uint16_t port)
{
    atci_status_t ret = ATCI_STATUS_ERROR;

    ret = atci_local_init();

    if (ret != ATCI_STATUS_OK) {
       // ret = atci_port_deinit(g_atci_port_handle);
        return ret;
    }
    #if defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)
    ret = atci_serial_port_init((serial_port_dev_t)port);
    #else
    ret = atci_uart_init((hal_uart_port_t)port);
    if(ret == ATCI_STATUS_OK)
        ret = atci_init_int();
    #endif

    return ret;
}

uint32_t atci_port_send_data(uint32_t port, uint8_t* buf, uint32_t buf_len)
{
    uint32_t data_len = 0;

#if defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)
    data_len = atci_serial_port_send_data(port, buf, buf_len);
#else
    data_len = atci_uart_send_data((hal_uart_port_t)port, buf, buf_len);	
#endif

    return data_len;
}

uint32_t atci_port_read_data(uint32_t port, uint8_t *buf, uint32_t buf_len)
{
    uint32_t data_len = 0;

#if defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)
    data_len = atci_serial_port_read_data(port, buf, buf_len);
#else
    data_len = atci_uart_read_data((hal_uart_port_t)port, buf, buf_len);
#endif

    return data_len;
}

atci_status_t atci_send_data_int(uint8_t* data, uint32_t data_len)
{
    atci_status_t ret = ATCI_STATUS_OK;
    uint32_t sent_len = 0;

    if(atci_vaild_port_check(g_atci_port_handle) == ATCI_STATUS_ERROR)
        return ATCI_STATUS_ERROR;

    sent_len = atci_port_send_data(g_atci_port_handle, data, data_len);

    if (sent_len == 0)
	{
        ret = ATCI_STATUS_ERROR;
        LOGE("atci_send_data_int() send data fail\r\n");
    }

    LOGI("atci_send_data_int() send data len:%d\r\n", sent_len);
    return ret;
}

atci_status_t atci_send_data(uint8_t* data, uint32_t data_len)
{
    uint8_t* send_data = NULL;
    uint32_t send_len = 0;
    uint32_t sent_len = 0;
    atci_status_t ret = ATCI_STATUS_OK;

    LOGI("atci_send_data() enter function, cache_left_len:%d \r\n", g_send_data_cache_left_len);
    if(atci_vaild_port_check(g_atci_port_handle) == ATCI_STATUS_ERROR)
    {
        LOGE("atci_send_data() send data fail,because g_atci_port_handle is invalid:%d \r\n", g_atci_port_handle);
        return ATCI_STATUS_ERROR;
    }

    if (g_send_data_cache_left_len == 0){

        send_data = data;
        send_len = data_len;
    }
    else if ((data == NULL) && (g_send_data_cache_left_len > 0)){

        if(g_send_data_cache != NULL)
        {
            send_data = &g_send_data_cache[g_send_data_cache_len - g_send_data_cache_left_len]; 
            send_len = g_send_data_cache_left_len;
        }
        else
            return  ATCI_STATUS_ERROR;
    }
    else
        return	ATCI_STATUS_ERROR;

    // send data by port
    sent_len = atci_port_send_data(g_atci_port_handle, send_data, send_len);
    if (sent_len == 0)
    {
        ret = ATCI_STATUS_ERROR;
        LOGE("atci_send_data() send data fail\r\n");
    }

    LOGI("atci_send_data() sending data(len:%d), sent data(len:%d)\r\n", send_len, sent_len);

    if(sent_len >= send_len) {
        // send all the input data
        if(g_send_data_cache != NULL)
        {
            atci_mem_free(g_send_data_cache);
            g_send_data_cache = NULL;
            LOGI("atci_send_data() atci_mem_free\r\n");
        }

        g_send_data_cache_len = 0;
        g_send_data_cache_left_len = 0;

        if(atci_input_commad_in_processing == 3)
        {
            atci_input_commad_in_processing = 0;
            LOGI("atci_send_data() send AT cmd response done\r\n");
        }
        else
            LOGI("atci_send_data() send data done\r\n");

		return ATCI_STATUS_OK;
    }
    else {
        // cache the left sending data
        if(g_send_data_cache_len == 0) {

            g_send_data_cache_len = send_len - sent_len;

            if(g_send_data_cache == NULL)
            {
                g_send_data_cache = (uint8_t *)atci_mem_alloc(g_send_data_cache_len);
                if(g_send_data_cache == NULL)
                {
                    LOGW("atci_send_data() alloc memory fail, drop the cached data(data_len:%d)\r\n", g_send_data_cache_left_len);
                    g_send_data_cache_len = 0;
                    g_send_data_cache_left_len = 0;
                    return ATCI_STATUS_ERROR;
                }
            }

            memset(g_send_data_cache, 0, g_send_data_cache_len);
            memcpy(g_send_data_cache, &send_data[sent_len], g_send_data_cache_len);
        }

        g_send_data_cache_left_len = send_len - sent_len;

        LOGI("atci_send_data() cached data(data_len:%d)\r\n", g_send_data_cache_left_len);
    }

    return ret;
}

atci_status_t atci_read_data(void)
{
    atci_status_t ret = ATCI_STATUS_OK;
    atci_send_input_cmd_msg_t input_data;
    atci_response_t   response = {{0}};
    uint32_t  length = 0;
    uint32_t find_valid_command = 0;

    LOGI("atci_read_data, enter,g_atci_port_handle:%d\r\n", g_atci_port_handle);
    if(atci_vaild_port_check(g_atci_port_handle) == ATCI_STATUS_ERROR)
        return ATCI_STATUS_ERROR;
    // user should know the read_data size before reading
    input_data.input_len = atci_port_read_data(g_atci_port_handle, (uint8_t*)input_data.input_buf, ATCI_UART_RX_FIFO_BUFFER_SIZE);
    if (input_data.input_len == 0)
    {
        LOGE("atci_read_data() atci_port_read_data read fail \r\n");
        return ATCI_STATUS_ERROR;
    }

    // drop data if cmd is not processed
    if (atci_input_commad_in_processing != 0)
    {
        /* input AT command is too fast and returns "ERROR" directly. Set Output Buffer Param */
        sprintf((char *)response.response_buf, "ERROR drop this cmd, need wait the response of previous AT cmd to input the next cmd!\r\n");
        response.response_len = (uint16_t)strlen((const char *)response.response_buf);
        atci_send_data_int(response.response_buf, response.response_len);
        LOGW("atci_read_data() drop this cmd, need wait the response of previous AT cmd \r\n");
        return ATCI_STATUS_ERROR;
    }

	 /* append current receive data to the input buffer*/
    if ((input_data.input_len + g_input_command.input_len) < ATCI_UART_RX_FIFO_BUFFER_SIZE)
    {
        // just use a golbal buffer (g_input_command) to receive data because the mem alloc API can't be used in hal irq	
        memcpy(&g_input_command.input_buf[g_input_command.input_len], (char*)input_data.input_buf, input_data.input_len); 
        g_input_command.input_len += input_data.input_len;
        LOGI("current input cmd (len:%d):%s\r\n", g_input_command.input_len, g_input_command.input_buf);

    } else {
        /*invild command, input cmd is larger then max len, need drop this cmd*/
        atci_input_commad_in_processing = 0;
        memset(&g_input_command, 0, sizeof(atci_send_input_cmd_msg_t));
        //printf("atci_uart_irq, receive data len (%d) is larger than max len, drop this cmd\r\n", g_receive_command->input_len);

        sprintf((char *)response.response_buf, "ERROR, Input data len(%d) is larger than max len, drop this cmd!\r\n", g_input_command.input_len);
        response.response_len = (uint16_t)strlen((char *)response.response_buf);
        atci_send_data(response.response_buf, response.response_len);
        LOGW("atci_read_data() return fail, input AT cmd length is larger than %d \r\n", ATCI_UART_RX_FIFO_BUFFER_SIZE);			  
        return ATCI_STATUS_ERROR;
    }

    /* find '\0'/CR/LF to be the end of AT command */
    for ( length = 0; length < g_input_command.input_len; length++)
    {
        if (('\0' == g_input_command.input_buf[length]) ||
            (ATCI_CR_C == g_input_command.input_buf[length]) ||
            (ATCI_LF_C == g_input_command.input_buf[length]))
        {
            find_valid_command = 1;
            break;
        }
    }

    if(find_valid_command == 1)
    {
        /* find "AT" to be the beginning of AT command */
        if (('A' == g_input_command.input_buf[0] && 'T' == g_input_command.input_buf[1]) ||
        ('a' == g_input_command.input_buf[0] && 't' == g_input_command.input_buf[1])) {

            atci_input_commad_in_processing = 1;
        } else {

            atci_input_commad_in_processing = 0;
            memset(&g_input_command, 0, sizeof(atci_send_input_cmd_msg_t));

            sprintf((char *)response.response_buf, "ERROR, receive data is not AT cmd type, drop this cmd!\r\n");
            response.response_len = (uint16_t)strlen((char *)response.response_buf);
            atci_send_data(response.response_buf, response.response_len);;
            LOGW("atci_read_data() return fail, receive data is not AT cmd type, drop this cmd! \r\n");
            return ATCI_STATUS_ERROR;
        }
    }
    return ret;
}



// processing the read/send data
/* This function can't be reentry. It should be executed completely and than invoke this function again. */
void atci_processing(void)
{
    /* Queue number */
    uint16_t            common_queue_msg_num = 0;
    uint16_t            notify_queue_msg_num = 0;
    uint16_t            cached_queue_msg_num = 0;
    atci_general_msg_t msg_queue_data;
    uint32_t data_len = 0;

    if (1) {// (g_atci_local_startup_flag == 0) {

        if (NULL == (void *)g_atci_processing_semaphore) {

        g_atci_processing_semaphore = atci_semaphore_create(10, 0);
            if (NULL == (void *)g_atci_processing_semaphore) {
                LOGE("atci_processing() create mutex fail\r\n");
            }
        } 
    }

    LOGI("atci_processing() enter\r\n");

    while (1) {
        //printf("murder task ATCI\n");
        if (( NULL != (void *)g_atci_handler_semaphore )&& (g_atci_local_startup_flag != 0))
        {
            LOGI("atci_processing() take AT handler mutex \r\n");
            atci_semaphore_take(g_atci_handler_semaphore);
        }
        else if ( (NULL != (void *)g_atci_processing_semaphore) && (g_atci_local_startup_flag == 0))
        {
            LOGI("atci_processing() take protect mutex \r\n");
            atci_semaphore_take(g_atci_processing_semaphore);
        }

        if(g_atci_local_startup_flag == 2)
            continue;

        common_queue_msg_num = atci_queue_get_item_num(g_atci_input_command_queue);
        /*
         * We are handling AT command one by one. Need to wait the previous AT command response and then handle the next one.
         * atci_input_commad_in_processing means the response of AT command was resonse or not.
         * When atci_input_commad_in_processing = 0 means we could receiveing AT comman
         * atci_input_commad_in_processing = 1 means receiving vaild AT command.
         * atci_input_commad_in_processing = 2 means parsing the  AT command.
         * atci_input_commad_in_processing = 3 means handling the AT command response.
         */

            if (common_queue_msg_num > 0) {
            LOGI("atci_processing() commad_in_processing(%d), input_queue_msg_num(%d)\r\n", atci_input_commad_in_processing, common_queue_msg_num);

            atci_queue_receive_no_wait(g_atci_input_command_queue,&msg_queue_data);

            LOGI("atci_processing() msg_id(%d)\\r\n", msg_queue_data.msg_id);

            /* message handling */
            if (MSG_ID_ATCI_LOCAL_SEND_CMD_IND == msg_queue_data.msg_id) {
                atci_read_data();

                if (atci_input_commad_in_processing == 1) {

                    /* handling someone AT command. Need to wait the response and then could handle the next AT command */
                    LOGI("atci_processing() handle AT input\r\n");
                    atci_input_commad_in_processing = 2;

                    atci_input_command_handler(&g_input_command);
                    /* g_input_command is available now */
                    memset(&g_input_command, 0, sizeof(atci_send_input_cmd_msg_t));
                }
            }
            common_queue_msg_num--;
        }

        if(g_atci_local_startup_flag == 2)
            continue;

        cached_queue_msg_num = atci_queue_get_item_num(g_atci_data_cached_queue);

        if(cached_queue_msg_num >0)
            LOGI("atci_processing() cached_queue_msg_num(%d)\r\n", cached_queue_msg_num);

        while (cached_queue_msg_num > 0) {

            atci_queue_receive_no_wait(g_atci_data_cached_queue, &msg_queue_data);
            if (msg_queue_data.msg_id == MSG_ID_ATCI_LOCAL_WRITE_CMD_IND) {
                // send the cache output data
                LOGI("atci_processing() g_send_data_cache_left_len(%d)\r\n", g_send_data_cache_left_len);
                if (g_send_data_cache_left_len > 0)
                    atci_send_data(NULL, 0);
            }
            cached_queue_msg_num --;
        }

        if(g_atci_local_startup_flag == 2)
            continue;

        notify_queue_msg_num = atci_queue_get_item_num(g_atci_resonse_command_queue);
        if(notify_queue_msg_num >0)
            LOGI("atci_processing() notify_queue_msg_num(%d)\r\n", notify_queue_msg_num);

        while ((g_send_data_cache_left_len == 0 ) && (notify_queue_msg_num > 0)) {
            int32_t ret = atci_queue_receive_no_wait(g_atci_resonse_command_queue,&msg_queue_data);
            if (0 == ret) {
                LOGE("atci_processing() get response queue data fail\r\n");
                return;
            }

            if ((MSG_ID_ATCI_LOCAL_RSP_NOTIFY_IND == msg_queue_data.msg_id) && (atci_input_commad_in_processing == 2)) {
                /* the response resonse of handling AT command is finished . allow to handle the next AT command */
                atci_input_commad_in_processing = 3;
                LOGI("atci_processing() handle AT response\r\n");
            }
            else if (MSG_ID_ATCI_LOCAL_URC_NOTIFY_IND == msg_queue_data.msg_id)
                LOGI("atci_processing() handle AT urc \r\n");
            /* the message handling. Sending response or URC to UART */
            memcpy(&data_len, msg_queue_data.msg_data, sizeof(uint16_t));
            atci_send_data((uint8_t *)(msg_queue_data.msg_data + sizeof(uint16_t)), data_len);
            /* free the message content */
            atci_mem_free(msg_queue_data.msg_data);
            msg_queue_data.msg_data = NULL;
            msg_queue_data.msg_id = MSG_ID_ATCI_END;

            LOGI("atci_processing() handle AT send cmd done\r\n");

            notify_queue_msg_num--;
        }
    }
}

atci_status_t atci_data_processing_for_deinit(void)
{
    /* Queue number */
    uint16_t            cached_queue_msg_num = 0;
    uint16_t            notify_queue_msg_num = 0;
    atci_general_msg_t msg_queue_data;
    atci_status_t ret =  ATCI_STATUS_ERROR;
    uint32_t data_len = 0;

    cached_queue_msg_num = atci_queue_get_item_num(g_atci_data_cached_queue);

    if(cached_queue_msg_num >0)
        LOGI("atci_data_processing_for_deinit() cached_queue_msg_num(%d)\r\n", cached_queue_msg_num);

    while (cached_queue_msg_num > 0) {

        atci_queue_receive_no_wait(g_atci_data_cached_queue, &msg_queue_data);
        if (msg_queue_data.msg_id == MSG_ID_ATCI_LOCAL_WRITE_CMD_IND) {
            // send the cache output data
            if (g_send_data_cache_left_len > 0)
                atci_send_data(NULL, 0);
        }
        cached_queue_msg_num --;
    }

    notify_queue_msg_num = atci_queue_get_item_num(g_atci_resonse_command_queue);
    if(notify_queue_msg_num >0)
        LOGI("atci_data_processing_for_deinit() notify_queue_msg_num(%d)\r\n", notify_queue_msg_num);

    while ((g_send_data_cache_left_len == 0 ) && (notify_queue_msg_num > 0)) {
        int32_t ret = atci_queue_receive_no_wait(g_atci_resonse_command_queue,&msg_queue_data);
        if (0 == ret) {
            LOGE("atci_processing() get response queue data fail\r\n");
            return ATCI_STATUS_ERROR;
        }

        if ((MSG_ID_ATCI_LOCAL_RSP_NOTIFY_IND == msg_queue_data.msg_id) && (atci_input_commad_in_processing == 2)) {
            /* the response resonse of handling AT command is finished . allow to handle the next AT command */
            atci_input_commad_in_processing = 3;
            LOGI("atci_data_processing_for_deinit() handle AT response\r\n");
        }
        else if (MSG_ID_ATCI_LOCAL_URC_NOTIFY_IND == msg_queue_data.msg_id)
            LOGI("atci_data_processing_for_deinit() handle AT urc \r\n");
        /* the message handling. Sending response or URC to UART */
        memcpy(&data_len, msg_queue_data.msg_data, sizeof(uint16_t));
        atci_send_data((uint8_t *)(msg_queue_data.msg_data + sizeof(uint16_t)), data_len);
        /* free the message content */
        atci_mem_free(msg_queue_data.msg_data);
        msg_queue_data.msg_data = NULL;
        msg_queue_data.msg_id = MSG_ID_ATCI_END;

        LOGI("atci_data_processing_for_deinit() handle AT send cmd done\r\n");

        notify_queue_msg_num--;

    }

    ret = ATCI_STATUS_OK;

    return ret;
}

atci_status_t atci_vaild_port_check(uint32_t port)
{
    atci_status_t ret = ATCI_STATUS_ERROR;

#if defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)
    if( port != 0)
        ret = ATCI_STATUS_OK;
#else
    if( port < HAL_UART_MAX)
        ret = ATCI_STATUS_OK;
#endif

    return ret;
}


atci_status_t atci_port_deinit(uint32_t port)
{
    atci_status_t ret = ATCI_STATUS_ERROR;

#if defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)
    ret = atci_serial_port_deinit(port);
#else
    ret = atci_uart_deinit((hal_uart_port_t)port);
#endif

    return ret;
}

atci_status_t atci_local_deinit(void)
{
    atci_status_t ret =  ATCI_STATUS_ERROR;
    uint16_t i;

    g_atci_local_startup_flag = 0;
    ret = atci_data_processing_for_deinit();

    atci_input_commad_in_processing = 0;

    g_atci_registered_table_number = 0;
    for ( i = 0; i < ATCI_MAX_GNENERAL_TABLE_NUM; i++) {
        g_atci_cm4_general_hdlr_tables[i].item_table = NULL;
        g_atci_cm4_general_hdlr_tables[i].item_table_size = 0;
    }

    memset(&g_input_command, 0, sizeof(atci_send_input_cmd_msg_t));

    ret = ATCI_STATUS_OK;

    return ret;
}


atci_status_t atci_deinit(hal_uart_port_t port)
{
    /* Init */
    atci_status_t ret = ATCI_STATUS_ERROR;

    LOGI("atci_deinit() enter (flag:%d)\r\n", g_atci_local_startup_flag);

    if (g_atci_local_startup_flag == 0) {
        return ret;
    }

    ret = atci_local_deinit();

	// close serial port
    ret = atci_port_deinit(g_atci_port_handle);
    if (ret != ATCI_STATUS_OK)
    {
        LOGE("atci_deinit() fail \r\n");
    }
    else
    {
        LOGI("atci_deinit() success \r\n");  
    }

    return ret;
}
	

