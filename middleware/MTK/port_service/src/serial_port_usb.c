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
#ifdef MTK_USB_DEMO_ENABLED
#include "syslog.h"
#include "usb.h"
#include "usb_custom_def.h"
#include "usbacm_drv.h"
#include "usbacm_adap.h"

serial_port_register_callback_t g_serial_port_usb_callback[MAX_USB_PORT_NUM] = {NULL};

/* Open USB2UART port */
serial_port_status_t serial_port_usb_init(serial_port_dev_t device, serial_port_open_para_t *para, void *priv_data)
{
    uint32_t usb_port;
    uint32_t length_tx = 0 , length_rx = 0;

    USB2UART_init();

    usb_port = USB_PORT[device];

    LOG_I(hal, "serial_port_usb_init, device:%d, usb_port:%d\n", device, usb_port);

    if (usb_port == 0) {
        length_tx = USB_TX_RING_BUFFER_1_SIZE;
        length_rx = USB_RX_RING_BUFFER_1_SIZE;
    } else if (usb_port == 1) {
        length_tx = USB_TX_RING_BUFFER_2_SIZE;
        length_rx = USB_RX_RING_BUFFER_2_SIZE;
    } else {
        LOG_I(hal, "ASSERT");
    }

    if (USB2UARTPort[usb_port].RingBuffers.rx_buffer == NULL) {
        USB2UARTPort[usb_port].RingBuffers.rx_buffer = (uint8_t *)USB_Get_Memory(length_rx);    //(uint8_t *)g_UsbACM_Comm.acm_param->rx_ringbuff_2;
    }

    Buf_init(&(USB2UARTPort[usb_port].Rx_Buffer), (uint8_t *)(USB2UARTPort[usb_port].RingBuffers.rx_buffer), length_rx);

    if (USB2UARTPort[usb_port].RingBuffers.tx_buffer == NULL) {
        USB2UARTPort[usb_port].RingBuffers.tx_buffer = (uint8_t *)USB_Get_Memory(length_tx);    //(uint8_t *)g_UsbACM_Comm.acm_param->tx_ringbuff_2;
    }
    Buf_init(&(USB2UARTPort[usb_port].Tx_Buffer), (uint8_t *)(USB2UARTPort[usb_port].RingBuffers.tx_buffer), length_tx);

    USB2UARTPort[usb_port].initialized = true;

    g_UsbACM[usb_port].send_Txilm = false;
    g_UsbACM[usb_port].send_Rxilm = true;
    g_UsbACM[usb_port].transfer_type = CDC_NORMAL;
    g_UsbACM[usb_port].config_send_Txilm = false;

    g_serial_port_usb_callback[usb_port] = para->callback;
    LOG_I(hal, "reg. usb_callback port:%d\n", usb_port);

    if (USB_Get_Device_State() != DEVSTATE_CONFIG) {
        LOG_I(hal, "serial_port_usb open NOT ready\n");
        return SERIAL_PORT_STATUS_DEV_NOT_READY;
    }

    if (g_UsbACM[usb_port].txpipe != NULL) {
        /* Only RX EP needs to be enabled since TX EP will use DMA polling */
        hal_usb_enable_tx_endpoint(g_UsbACM[usb_port].txpipe->byEP, HAL_USB_EP_TRANSFER_BULK, HAL_USB_EP_USE_ONLY_DMA, false);
        hal_usb_enable_rx_endpoint(g_UsbACM[usb_port].rxpipe->byEP, HAL_USB_EP_TRANSFER_BULK, HAL_USB_EP_USE_NO_DMA, false);

        /* Open intr */
        USB_UnMask_COM_Intr(device);
    }

    return SERIAL_PORT_STATUS_OK;
}


serial_port_status_t serial_port_usb_control(serial_port_dev_t dev, serial_port_ctrl_cmd_t cmd, serial_port_ctrl_para_t *para)
{
    serial_port_status_t ret = SERIAL_PORT_STATUS_OK;
    serial_port_write_data_t *serial_port_write_data;
    serial_port_read_data_t *serial_port_read_data;
    serial_port_get_write_avail_t *para_uart_get_tx_avail;
    serial_port_get_read_avail_t *para_uart_get_rx_avail;
    serial_port_write_data_t *serial_port_write_data_blocking;

    switch (cmd) {
        case SERIAL_PORT_CMD_WRITE_DATA: {
            serial_port_write_data = (serial_port_write_data_t *)para;
            serial_port_write_data->ret_size = USB2UART_PutBytes(dev, serial_port_write_data->data, serial_port_write_data->size);
            if (serial_port_write_data->ret_size == 0) {
                ret = SERIAL_PORT_STATUS_FAIL;
            }
        }
        break;
        case SERIAL_PORT_CMD_READ_DATA:
        case SERIAL_PORT_CMD_READ_DATA_BLOCKING: {
            serial_port_read_data = (serial_port_read_data_t *)para;
            serial_port_read_data->ret_size = USB2UART_GetBytes(dev, serial_port_read_data->buffer, serial_port_read_data->size);
            if (serial_port_read_data->ret_size == 0) {
                ret = SERIAL_PORT_STATUS_FAIL;
            }
        }
        break;
        case SERIAL_PORT_CMD_GET_WRITE_AVAIL: {
            para_uart_get_tx_avail = (serial_port_get_write_avail_t *)para;
            para_uart_get_tx_avail->ret_size = USB2UART_GetTxRoomLeft(dev);
            if (para_uart_get_tx_avail->ret_size == 0) {
                ret = SERIAL_PORT_STATUS_FAIL;
            }
        }
        break;
        case SERIAL_PORT_CMD_GET_READ_AVAIL: {
            para_uart_get_rx_avail = (serial_port_get_read_avail_t *)para;
            para_uart_get_rx_avail->ret_size = USB2UART_GetRxAvail(dev);
        }
        break;

        case SERIAL_PORT_CMD_WRITE_DATA_BLOCKING: {
            serial_port_write_data_blocking = (serial_port_write_data_t *)para;
            USB2UART_PutData_Polling(dev, serial_port_write_data_blocking->data, serial_port_write_data_blocking->size);
            serial_port_write_data_blocking->ret_size = serial_port_write_data_blocking->size;
            if (serial_port_write_data_blocking->ret_size == 0) {
                ret = SERIAL_PORT_STATUS_FAIL;
            }
        }
        break;

        default:
//			ASSERT(0);
            ret = SERIAL_PORT_STATUS_UNSUPPORTED;
            break;
    }
    return ret;
}

/* Close USB2UART port */
serial_port_status_t serial_port_usb_deinit(serial_port_dev_t port)
{
    uint32_t usb_port = USB_PORT[port];

    g_UsbACM[usb_port].send_Txilm = false;
    g_UsbACM[usb_port].config_send_Txilm = false;
    USB2UARTPort[usb_port].initialized = false;

    if ((g_UsbACM[usb_port].txpipe != NULL) && (USB_Get_Device_State() == DEVSTATE_CONFIG)) {
        /* After assert or fatal error, TST will put many 0x00 to ring buffer, then UART_close and UART_open */
        /* disable endpoint interrupt */

        hal_usb_stop_dma_channel(g_UsbACM[usb_port].txpipe->byEP, HAL_USB_EP_DIRECTION_TX);
        g_UsbACM[usb_port].setup_dma = false;

        hal_usb_disable_tx_endpoint(g_UsbACM[usb_port].txpipe->byEP);
        hal_usb_disable_rx_endpoint(g_UsbACM[usb_port].rxpipe->byEP);
    }

    g_UsbACM[usb_port].transfer_type = CDC_NORMAL;

    return SERIAL_PORT_STATUS_OK;
}
#else

serial_port_status_t serial_port_usb_init(serial_port_dev_t device, serial_port_open_para_t *para, void *priv_data)
{
    return SERIAL_PORT_STATUS_UNSUPPORTED;
}

serial_port_status_t serial_port_usb_control(serial_port_dev_t dev, serial_port_ctrl_cmd_t cmd, serial_port_ctrl_para_t *para)
{
    return SERIAL_PORT_STATUS_UNSUPPORTED;
}

serial_port_status_t serial_port_usb_deinit(serial_port_dev_t port)
{
    return SERIAL_PORT_STATUS_UNSUPPORTED;
}

#endif
#endif
