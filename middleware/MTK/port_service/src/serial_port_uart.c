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

#ifdef MTK_PORT_SERVICE_ENABLE

#include "serial_port.h"
#include "serial_port_internal.h"
#include "memory_attribute.h"

ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_serial_port_receive_vfifo_buffer[HAL_UART_MAX][SERIAL_PORT_RECEIVE_BUFFER_SIZE];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_serial_port_send_vfifo_buffer[HAL_UART_MAX][SERIAL_PORT_SEND_BUFFER_SIZE];

static serial_port_register_callback_t g_serial_port_register_callback[HAL_UART_MAX] = {NULL};
static serial_port_dev_t g_serial_port_uart_port[] = {
    SERIAL_PORT_DEV_UART_0,
    SERIAL_PORT_DEV_UART_1,
    SERIAL_PORT_DEV_UART_2,
    SERIAL_PORT_DEV_UART_3
};

static void serial_port_uart_callback(hal_uart_callback_event_t event, void *user_data)
{
    serial_port_dev_t uart_port = *(serial_port_dev_t *)user_data;

    if (g_serial_port_register_callback[uart_port] != NULL) {
        if (event == HAL_UART_EVENT_READY_TO_READ) {
            g_serial_port_register_callback[uart_port](uart_port, SERIAL_PORT_EVENT_READY_TO_READ, NULL);
        } else if (event == HAL_UART_EVENT_READY_TO_WRITE) {
            g_serial_port_register_callback[uart_port](uart_port, SERIAL_PORT_EVENT_READY_TO_WRITE, NULL);
        }
    }
}

serial_port_status_t serial_port_uart_init(serial_port_dev_t device, serial_port_open_para_t *para, void *priv_data)
{
    hal_uart_port_t uart_port;
    hal_uart_status_t status;
    hal_uart_config_t uart_config;
    hal_uart_dma_config_t dma_config;
    serial_port_setting_uart_t *port_setting;

    uart_port = (hal_uart_port_t)device;
    port_setting = (serial_port_setting_uart_t *)priv_data;

    uart_config.baudrate = port_setting->baudrate;
    uart_config.parity = HAL_UART_PARITY_NONE;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    status = hal_uart_init(uart_port, &uart_config);
    if (status != HAL_UART_STATUS_OK) {
        return SERIAL_PORT_STATUS_FAIL;
    }

    dma_config.receive_vfifo_alert_size = 20;
    dma_config.receive_vfifo_buffer = g_serial_port_receive_vfifo_buffer[uart_port];
    dma_config.receive_vfifo_buffer_size = SERIAL_PORT_RECEIVE_BUFFER_SIZE;
    dma_config.receive_vfifo_threshold_size = (SERIAL_PORT_RECEIVE_BUFFER_SIZE * 2) / 3;
    dma_config.send_vfifo_buffer = g_serial_port_send_vfifo_buffer[uart_port];
    dma_config.send_vfifo_buffer_size = SERIAL_PORT_SEND_BUFFER_SIZE;
    dma_config.send_vfifo_threshold_size = SERIAL_PORT_SEND_BUFFER_SIZE / 10;
    status = hal_uart_set_dma(uart_port, &dma_config);
    if (status != HAL_UART_STATUS_OK) {
        return SERIAL_PORT_STATUS_FAIL;
    }

    g_serial_port_register_callback[device] = para->callback;
    status = hal_uart_register_callback(uart_port, serial_port_uart_callback, &g_serial_port_uart_port[device]);
    if (status != HAL_UART_STATUS_OK) {
        return SERIAL_PORT_STATUS_FAIL;
    }

    return SERIAL_PORT_STATUS_OK;
}

serial_port_status_t serial_port_uart_control(serial_port_dev_t device, serial_port_ctrl_cmd_t cmd, serial_port_ctrl_para_t *para)
{
    hal_uart_port_t uart_port;
    serial_port_status_t ret = SERIAL_PORT_STATUS_OK;
    serial_port_write_data_t *serial_port_write_data;
    serial_port_read_data_t *serial_port_read_data;
    serial_port_get_write_avail_t *serial_port_get_write_avail;
    serial_port_get_read_avail_t *serial_port_get_read_avail;
    uint32_t size_left, size_eat;
    uint8_t *p_buf;

    uart_port = (hal_uart_port_t)device;
    ret = SERIAL_PORT_STATUS_OK;
    switch (cmd) {
        case SERIAL_PORT_CMD_WRITE_DATA:
            serial_port_write_data = (serial_port_write_data_t *)para;
            serial_port_write_data->ret_size = hal_uart_send_dma(uart_port, serial_port_write_data->data, serial_port_write_data->size);
            break;
        case SERIAL_PORT_CMD_READ_DATA:
            serial_port_read_data = (serial_port_read_data_t *)para;
            serial_port_read_data->ret_size = hal_uart_receive_dma(uart_port, serial_port_read_data->buffer, serial_port_read_data->size);
            break;
        case SERIAL_PORT_CMD_WRITE_DATA_BLOCKING:
            serial_port_write_data = (serial_port_write_data_t *)para;
            size_left = serial_port_write_data->size;
            p_buf = serial_port_write_data->data;
            while (size_left) {
                size_eat = hal_uart_send_dma(uart_port, p_buf, size_left);
                size_left -= size_eat;
                p_buf += size_eat;
            }
            while ((SERIAL_PORT_SEND_BUFFER_SIZE - hal_uart_get_available_send_space(uart_port)) != 0);
            serial_port_write_data->ret_size = serial_port_write_data->size;
            break;
        case SERIAL_PORT_CMD_READ_DATA_BLOCKING:
            serial_port_read_data = (serial_port_read_data_t *)para;
            size_left = serial_port_read_data->size;
            p_buf = serial_port_read_data->buffer;
            while (size_left) {
                size_eat = hal_uart_receive_dma(uart_port, p_buf, size_left);
                size_left -= size_eat;
                p_buf += size_eat;
            }
            serial_port_read_data->ret_size = serial_port_read_data->size;
            break;
        case SERIAL_PORT_CMD_GET_WRITE_AVAIL:
            serial_port_get_write_avail = (serial_port_get_write_avail_t *)para;
            serial_port_get_write_avail->ret_size = hal_uart_get_available_send_space(uart_port);
            break;
        case SERIAL_PORT_CMD_GET_READ_AVAIL:
            serial_port_get_read_avail = (serial_port_get_read_avail_t *)para;
            serial_port_get_read_avail->ret_size = hal_uart_get_available_receive_bytes(uart_port);
            break;
        default:
            ret = SERIAL_PORT_STATUS_UNSUPPORTED;
    }

    return ret;
}

serial_port_status_t serial_port_uart_deinit(serial_port_dev_t device)
{
    hal_uart_port_t uart_port;
    hal_uart_status_t status;

    uart_port = (hal_uart_port_t)device;

    status = hal_uart_deinit(uart_port);
    if (status != HAL_UART_STATUS_OK) {
        return SERIAL_PORT_STATUS_FAIL;
    }

    return SERIAL_PORT_STATUS_OK;
}
#endif

