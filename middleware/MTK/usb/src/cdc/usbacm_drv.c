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

#ifdef MTK_USB_DEMO_ENABLED

#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "memory_attribute.h"
#include "hal_usb.h"
#include "usb_comm.h"
#include "usb.h"
#include "usb_resource.h"
#include "usbacm_drv.h"
#include "usbacm_adap.h"

#ifdef MTK_PORT_SERVICE_ENABLE
#include "serial_port.h"
#endif


/* Global variables */
UsbAcm_Common_Struct g_UsbACM_Comm;

UsbUARTStruct USB2UARTPort[MAX_USB_PORT_NUM];
UsbAcm_Struct g_UsbACM[MAX_USB_PORT_NUM];

#ifdef MTK_PORT_SERVICE_ENABLE
uint8_t USB_PORT[SERIAL_PORT_DEV_MAX];
#else
uint8_t USB_PORT[2];
#endif


/* for DMA tx buffer and tx_isr buffer to differentiate */
static uint32_t USB2Uart_WriteLength[MAX_USB_PORT_NUM];

/* Static functions */
static uint8_t USB_Acm_Get_DataIf_Num(uint8_t port);
void USB_Acm_CommIf_Create(uint8_t port, void *ifname);
void USB_Acm_CommIf_Reset(uint8_t port);
void USB_Acm_CommIf_Enable(uint8_t port);
void USB_Acm_CommIf_Speed_Reset(uint8_t port, bool b_other_speed);
void USB_Acm_DataIf_Create(uint8_t port, void *ifname);
void USB_Acm_DataIf_Reset(uint8_t port);
void USB_Acm_DataIf_Enable(uint8_t port);
void USB_Acm_DataIf_Speed_Reset(uint8_t port, bool b_other_speed);
void USB_Acm_Break_Timeout(uint8_t port, void *parameter);
void USB_Acm_Ep0_SetLineCoding(uint8_t port, void *data);
void USB_Acm_Ep0_Command(uint8_t port, Usb_Ep0_Status *pep0state, Usb_Command *pcmd);
void USB_Acm_BulkOut_Hdr(uint8_t port);
void USB_Acm_BulkIn_Reset(uint8_t port);
void USB_Acm_BulkOut_Reset(uint8_t port);
void USB_Acm_IntrIn_Hdlr(uint8_t port);
void USB_Acm_IntrIn_Reset(uint8_t port);


void USB_Acm1_CommIf_Create(void *ifname)
{
    USB_Acm_CommIf_Create(0, ifname);
}

void USB_Acm1_CommIf_Reset(void)
{
    USB_Acm_CommIf_Reset(0);
}

void USB_Acm1_CommIf_Enable(void)
{
    USB_Acm_CommIf_Enable(0);
}

void USB_Acm1_CommIf_Speed_Reset(bool b_other_speed)
{
    USB_Acm_CommIf_Speed_Reset(0, b_other_speed);

}

void USB_Acm1_DataIf_Create(void *ifname)
{
    USB_Acm_DataIf_Create(0, ifname);
}

void USB_Acm1_DataIf_Reset(void)
{
    USB_Acm_DataIf_Reset(0);
}

void USB_Acm1_DataIf_Speed_Reset(bool b_other_speed)
{
    USB_Acm_DataIf_Speed_Reset(0, b_other_speed);
}

void USB_Acm1_DataIf_Enable(void)
{
    USB_Acm_DataIf_Enable(0);
}


/*
static void USB_Acm1_Ep0_SetLineCoding(void *data)
{
	USB_Acm_Ep0_SetLineCoding(0, data);
}
*/

static void USB_Acm1_Ep0_Command(Usb_Ep0_Status *pep0state, Usb_Command *pcmd)
{
    USB_Acm_Ep0_Command(0, pep0state, pcmd);
}

static void USB_Acm1_BulkOut_Hdr(void)
{
    USB_Acm_BulkOut_Hdr(0);
}

static void USB_Acm1_BulkIn_Reset(void)
{
    USB_Acm_BulkIn_Reset(0);
}

static void USB_Acm1_BulkOut_Reset(void)
{
    USB_Acm_BulkOut_Reset(0);
}

/*
static void USB_Acm1_IntrIn_Hdlr(void)
{
    USB_Acm_IntrIn_Hdlr(0);
}
*/

static void USB_Acm1_IntrIn_Reset(void)
{
    USB_Acm_IntrIn_Reset(0);
}

void USB_Acm2_CommIf_Create(void *ifname)
{
    USB_Acm_CommIf_Create(1, ifname);
}

void USB_Acm2_CommIf_Reset(void)
{
    USB_Acm_CommIf_Reset(1);
}

void USB_Acm2_CommIf_Enable(void)
{
    USB_Acm_CommIf_Enable(1);
}

void USB_Acm2_CommIf_Speed_Reset(bool b_other_speed)
{
    USB_Acm_CommIf_Speed_Reset(1, b_other_speed);
}

void USB_Acm2_DataIf_Create(void *ifname)
{
    USB_Acm_DataIf_Create(1, ifname);
}

void USB_Acm2_DataIf_Reset(void)
{
    USB_Acm_DataIf_Reset(1);
}

void USB_Acm2_DataIf_Enable(void)
{
    USB_Acm_DataIf_Enable(1);
}

void USB_Acm2_DataIf_Speed_Reset(bool b_other_speed)
{
    USB_Acm_DataIf_Speed_Reset(1, b_other_speed);
}

static void USB_Acm2_Ep0_Command(Usb_Ep0_Status *pep0state, Usb_Command *pcmd)
{
    USB_Acm_Ep0_Command(1, pep0state, pcmd);
}
/*
static void USB_Acm2_IntrIn_Hdlr(void)
{
    USB_Acm_IntrIn_Hdlr(1);
}
*/
static void USB_Acm2_IntrIn_Reset(void)
{
    USB_Acm_IntrIn_Reset(1);
}

static void USB_Acm2_BulkOut_Hdr(void)
{
    USB_Acm_BulkOut_Hdr(1);
}

static void USB_Acm2_BulkIn_Reset(void)
{
    USB_Acm_BulkIn_Reset(1);
}
static void USB_Acm2_BulkOut_Reset(void)
{
    USB_Acm_BulkOut_Reset(1);
}
static const uint8_t cdcacm_com_if_dscr[] = {
    USB_IFDSC_LENGTH,	//bLength;
    USB_INTERFACE,	//bDescriptorType;
    0x00,	//bInterfaceNumber;
    0x00,	//bAlternateSetting;
    USB_CDC_ACM_COMM_EP_NUMBER,	//bNumEndpoints;
    USB_ACM_COMM_INTERFACE_CLASS_CODE,	//bInterfaceClass;
    USB_ACM_COMM_INTERFACE_SUBCLASS_CODE,	//bInterfaceSubClass;
    USB_ACM_COMM_INTERFACE_PROTOCOL_CODE,	//bInterfaceProtocol;
    0x00,	//iInterface;

    0x05,	//HFbFunctionLength;		/*Header Functional Descriptor*/
    0x24,	//HFbDescriptorType;
    0x00,	//HFbDescriptorSubtype;
    0x10,	//bcdCDC;
    0x01,	//bcdCDC;

    0x04,	//ACMFbFunctionLength;	/*Abstract Control Management Functional Descriptor*/
    0x24,	//ACMFbDescriptorType;
    0x02,	//ACMFbDescriptorSubtype;
    0x0f,	//ACMFbmCapabilities;

    0x05,	//UFbFunctionLength;		/*Union Functional Descriptor*/
    0x24,	//UFbDescriptorType;
    0x06,	//UFbDescriptorSubtype;
    0x00,	//bMasterInterface;
    0x00,	//bSlaveInterface0;

    0x05,	//CMFbFunctionLength;	/*Call Management Descriptor*/
    0x24,	//CMFbDescriptorType;
    0x01,	//CMFbDescriptorSubtype;
    0x03,	//CMFbmCapabilities;
    0x00	//bDataInterface;
};

const uint8_t cdcacm_intr_dscr[] = {
    USB_EPDSC_LENGTH,	//bLength;
    USB_ENDPOINT,	//bDescriptorType;
    0x00,	//bEndpointAddress;
    USB_EP_INTR,	//bmAttributes;
    HAL_USB_MAX_PACKET_SIZE_ENDPOINT_INTERRUPT_HIGH_SPEED & 0xff,	//wMaxPacketSize[2];
    0x00,	//wMaxPacketSize[2];
    0x03	//bInterval;
};



const uint8_t cdcacm_data_if_dscr[] = {
    USB_IFDSC_LENGTH,	//bLength;
    USB_INTERFACE,	//bDescriptorType;
    0,	//bInterfaceNumber;
    0,	//bAlternateSetting;
    USB_CDC_ACM_DATA_EP_NUMBER,	//bNumEndpoints;
    USB_ACM_DATA_INTERFACE_CLASS_CODE,	//bInterfaceClass;
    USB_ACM_DATA_INTERFACE_SUBCLASS_CODE,	//bInterfaceSubClass;
    USB_ACM_DATA_INTERFACE_PROTOCOL_CODE,	//bInterfaceProtocol;
    0	//iInterface;
};


const uint8_t cdcacm_ep_in_dscr[] = {
    USB_EPDSC_LENGTH,	//bLength;
    USB_ENDPOINT,	//bDescriptorType;
    0x00,	//bEndpointAddress;
    USB_EP_BULK,	//bmAttributes;
    0x40,	//wMaxPacketSize[2];
    0x00,	//wMaxPacketSize[2];
    0x00	//bInterval;
};

const uint8_t cdcacm_ep_out_dscr[] = {
    USB_EPDSC_LENGTH,	//bLength;
    USB_ENDPOINT,	//bDescriptorType;
    0x00,	//bEndpointAddress;
    USB_EP_BULK,	//bmAttributes;
    0x40,	//wMaxPacketSize[2];
    0x00,	//wMaxPacketSize[2];
    0x00	//bInterval;
};


void USB_UnMask_COM_Intr(uint32_t port)
{
    uint32_t usb_port = USB_PORT[port];
    if (g_UsbACM[usb_port].rxpipe != NULL) {
        //USB_RxEPEn(g_UsbACM[usb_port].rxpipe->byEP, USB_ENDPT_BULK, USB_EP_USE_NO_DMA, false, true);
        hal_usb_enable_rx_endpoint(g_UsbACM[usb_port].rxpipe->byEP, HAL_USB_EP_TRANSFER_BULK, HAL_USB_EP_USE_NO_DMA, true);
    }
}


void USB_Mask_COM_Intr(uint32_t port)
{
    uint32_t usb_port = port;
    if (g_UsbACM[usb_port].rxpipe != NULL) {
        //USB_RxEPDis(g_UsbACM[usb_port].rxpipe->byEP, USB_EP_USE_NO_DMA);
        hal_usb_disable_rx_endpoint(g_UsbACM[usb_port].rxpipe->byEP);
    }
}

static hal_usb_dma_handler_t USB_COM_DMA_Callback(uint32_t port)
{

#ifdef MTK_PORT_SERVICE_ENABLE
    if (port == SERIAL_PORT_DEV_USB_COM1) {
        return  USB2UART_Tx_DMA1_Callback;
    } else if (port == SERIAL_PORT_DEV_USB_COM2) {
        return  USB2UART_Tx_DMA2_Callback;
    }
    return NULL;
#else
    if (port == 0) {
        return	USB2UART_Tx_DMA1_Callback;
    } else if (port == 1) {
        return	USB2UART_Tx_DMA2_Callback;
    }
    return NULL;
#endif

}


/*************************************************************
  translate ACM and UART setting functions
**************************************************************/

#if 0
/* Translate ACM line coding to UART DCB */
static void USB2UART_LineCoding2DCB(UsbAcm_Line_Coding *pline_coding, UARTDCBStruct *pDCB)
{
    pDCB->baud = (UART_baudrate)(pline_coding->dwDTERate);
    pDCB->stopBits = (UART_stopBits)(pline_coding->bCharFormat + 1);
    pDCB->parity = (UART_parity)(pline_coding->bParityType);
    pDCB->dataBits = (UART_bitsPerCharacter)(pline_coding->bDataBits);
}


/* Translate UART DCB to ACM line coding */
void UART2USB_DCB2LineCoding(UARTDCBStruct *pDCB, UsbAcm_Line_Coding *pline_coding)
{
    pline_coding->dwDTERate = (uint32_t)(pDCB->baud);
    pline_coding->bCharFormat = (uint8_t)(pDCB->stopBits) - 1;
    pline_coding->bParityType = (uint8_t)(pDCB->parity);
    pline_coding->bDataBits = (uint8_t)(pDCB->dataBits);
}
#endif

/************************************************************
	Default UART callback function. Send ilm to UART owner
*************************************************************/
#ifdef MTK_PORT_SERVICE_ENABLE
extern serial_port_register_callback_t g_serial_port_usb_callback[MAX_USB_PORT_NUM];
#endif

void USB2UART_Dafault_Tx_Callback(uint32_t port)
{
    uint32_t usb_port = USB_PORT[port];

    if (g_UsbACM[usb_port].send_Txilm == true) {
#ifdef MTK_PORT_SERVICE_ENABLE
        /* Send message to UART owner */
        if ( g_serial_port_usb_callback[usb_port] != NULL ) {
            g_serial_port_usb_callback[usb_port]((serial_port_dev_t)port, SERIAL_PORT_EVENT_READY_TO_WRITE, NULL);
        }
#else
#endif
        g_UsbACM[usb_port].send_Txilm = false;
        g_UsbACM[usb_port].config_send_Txilm = false;
    }
}


void USB2UART_Dafault_Rx_Callback(uint32_t port)
{
    uint32_t usb_port = port;//USB_PORT[port];

    if (g_UsbACM[usb_port].send_Rxilm == true) {
#ifdef MTK_PORT_SERVICE_ENABLE
        /* Send ready to read message to UART onwer if needed */
        if ( g_serial_port_usb_callback[usb_port] != NULL ) {
            if (usb_port == 0) {
                g_serial_port_usb_callback[usb_port](SERIAL_PORT_DEV_USB_COM1, SERIAL_PORT_EVENT_READY_TO_READ, NULL);
            } else if (usb_port == 1) {
                g_serial_port_usb_callback[usb_port](SERIAL_PORT_DEV_USB_COM2, SERIAL_PORT_EVENT_READY_TO_READ, NULL);
            }
        }
#else
#endif
        //g_UsbACM[usb_port].send_Rxilm = false;
    }
}

/************************************************************
	Bulk EP IN handle functions (DMA setup and callback functions)
*************************************************************/

/*
    Determine transmit data
    If return value is large than 0, the caller should send the data in parameter.
     This function and "USB2UART_Update_Transmit_Data" function must be pair.
*/
static uint32_t USB2UART_Check_Transmit_Data(uint32_t port, uint32_t *addr)
{
    uint32_t usb_port = USB_PORT[port];
    uint32_t length = 0;
    uint32_t savedMask;

    savedMask = SaveAndSetIRQMask();

    /* Check if TX buffer has enough data to send out */
    if ((USB2UARTPort[usb_port].Tx_Buffer.Write != USB2UARTPort[usb_port].Tx_Buffer.Read) && (length == 0)) {
        *addr = (uint32_t)USB2UARTPort[usb_port].Tx_Buffer.CharBuffer + USB2UARTPort[usb_port].Tx_Buffer.Read;

        if (USB2UARTPort[usb_port].Tx_Buffer.Write >= USB2UARTPort[usb_port].Tx_Buffer.Read) {
            length = USB2UARTPort[usb_port].Tx_Buffer.Write - USB2UARTPort[usb_port].Tx_Buffer.Read;
        } else {
            length = USB2UARTPort[usb_port].Tx_Buffer.Length - USB2UARTPort[usb_port].Tx_Buffer.Read;
        }
    }
    RestoreIRQMask(savedMask);

    /* If the packet size is multiple of 64, make the last one to be less than 64.
    	Otherwise the last packet may not be seen on WinXP */
    if (((length & 0x3f) == 0) && (length != 0)) {
        USB2Uart_WriteLength[usb_port] = length - 1;
    } else {
        USB2Uart_WriteLength[usb_port] = length;
    }

    return USB2Uart_WriteLength[usb_port];
}

/*
    Update the buffer information after transmit done
    Note that "USB2Uart_WriteLength" will be reset to 0.
    This function and "USB2UART_Check_Transmit_Data" function must be pair.
*/

static void USB2UART_Update_Transmit_Data(uint32_t port)
{
    uint32_t usb_port = USB_PORT[port];

    /* Update buffer information */
    {
        USB2UARTPort[usb_port].Tx_Buffer.Read += USB2Uart_WriteLength[usb_port];
        if (!(USB2UARTPort[usb_port].Tx_Buffer.Read <= USB2UARTPort[usb_port].Tx_Buffer.Length)) {
            LOG_E(hal, "USB2UART_Update_Transmit_Data, ASSERT\n");
        }

        if (USB2UARTPort[usb_port].Tx_Buffer.Read == USB2UARTPort[usb_port].Tx_Buffer.Length) {
            USB2UARTPort[usb_port].Tx_Buffer.Read = 0;
        }
    }

    /* Reset to 0*/
    USB2Uart_WriteLength[usb_port] = 0;
}

/*
    Flush all data in TX ring buffer, use polling method
    Note that this is special for sending exception log since interrupt is disabled when exception occurs
    It must not be used in normal time
*/
void USB2UART_Polling_Flush_Transmit_Data(uint32_t port)
{
#if 0
    uint32_t usb_port = USB_PORT[port];
    uint32_t addr;
    uint32_t length = 0;
    /* Avoid sending ilm to UART owner */
    g_UsbACM[usb_port].send_Txilm = false;
    g_UsbACM[usb_port].send_Rxilm = false;

    /* Wait for the running DMA done */
    USB_Polling_Transmit_Done(g_UsbACM[usb_port].txpipe->byEP, USB_ENDPT_BULK);

    /* Flush the data in TX buffer */
    length = USB2UART_Check_Transmit_Data(port, &addr);

    while (length) {
        /* used for callback function to know sent bytes */
        USB_Polling_Transmit_Data(g_UsbACM[usb_port].txpipe->byEP, USB_EP_TX_DIR, USB_ENDPT_BULK, addr, length, NULL, false, true, USB_DMA1_TYPE);
        USB2UART_Update_Transmit_Data(port);
        length = USB2UART_Check_Transmit_Data(port, &addr);
        if (USB_Get_Device_State() != DEVSTATE_CONFIG) {
            length = 0;
            break;
        }
    }
#endif
}

extern uint8_t test_data[];


/* Determine what buffer has data to send and call DMA setup function */
void USB2UART_DMATransmit(uint32_t port)
{
    uint32_t usb_port = USB_PORT[port];
    uint32_t addr = 0;
    uint32_t length = 0;
    uint8_t ep_num = 0;
    uint32_t savedMask;
    bool usb_dma_setup = false;
    hal_usb_dma_handler_t dma_callback;

    length = USB2UART_Check_Transmit_Data(port, &addr);

    if (length != 0) {
        savedMask = SaveAndSetIRQMask();
        /* In case USB is plugged out just before this critical section */
        if (g_UsbACM[usb_port].txpipe != NULL) {
            ep_num = g_UsbACM[usb_port].txpipe->byEP;
            usb_dma_setup = true;
        }
        RestoreIRQMask(savedMask);
        if (usb_dma_setup == true) {

            dma_callback = USB_COM_DMA_Callback(port);
            /* Use short packet to instead of "max_packet + ZLP" */

            if (length % HAL_USB_MAX_PACKET_SIZE_ENDPOINT_BULK_FULL_SPEED == 0) {
                length -= 1;
                USB2Uart_WriteLength[usb_port] -= 1;
            }
            hal_usb_start_dma_channel(ep_num, HAL_USB_EP_DIRECTION_TX, HAL_USB_EP_TRANSFER_BULK, (void *)addr, length, dma_callback, false, HAL_USB_DMA1_TYPE);
        }
        /* else part must because of cable out, so we do not need to care */
    } else {
        g_UsbACM[usb_port].setup_dma = false;
        if (g_UsbACM[usb_port].put_start == true) {
            g_UsbACM[usb_port].dmaf_setmember |= 0x02;
        }
    }
}


/* DMA callback function for TX sent out data */
void USB2UART_Tx_DMA_Callback(uint32_t port)
{
    uint32_t usb_port = USB_PORT[port];
    uint32_t savedMask;
    bool setup_dma = false;

    // LOG_I(hal, "USB2UART_Tx_DMA_Callback, usb port = %d", usb_port);

    /*It can only be clear by USB2UART_PutBytes*/


    /* TX complete callback */
    USB2UARTPort[usb_port].tx_cb(port);

    {
        /* Update read pointer for previously sent out buffer
            As for race condition, so we update transmit data first.
            The putISRbyte may be called by LISR or HISR*/
        savedMask = SaveAndSetIRQMask();
        USB2UART_Update_Transmit_Data(port);
        RestoreIRQMask(savedMask);

        /* USB2Uart_MemType and USB2Uart_WriteLength are updated, so clear dma running state here*/
        g_UsbACM[usb_port].setup_dma = false;
        if (g_UsbACM[usb_port].put_start == true) {
            g_UsbACM[usb_port].dmaf_setmember |= 0x01;
        }

        /* send data again in TX buffer or TXISR buffer */
        savedMask = SaveAndSetIRQMask();
        /* In case USB is plugged out just before this critical section */
        if (g_UsbACM[usb_port].txpipe != NULL) {
            if (g_UsbACM[usb_port].setup_dma == false) {
                g_UsbACM[usb_port].setup_dma = true;
                setup_dma = true;
            }
        }
        RestoreIRQMask(savedMask);

        if (setup_dma == true) {
            USB2UART_DMATransmit(port);
            /* There are already some logs in USB2UART_DMATransmit() */

        }
    }
}



void USB2UART_Tx_DMA1_Callback(void)
{
#ifdef MTK_PORT_SERVICE_ENABLE
    USB2UART_Tx_DMA_Callback(SERIAL_PORT_DEV_USB_COM1);
#else
    USB2UART_Tx_DMA_Callback(0);
#endif

}

void USB2UART_Tx_DMA2_Callback(void)
{
#ifdef MTK_PORT_SERVICE_ENABLE
    USB2UART_Tx_DMA_Callback(SERIAL_PORT_DEV_USB_COM2);
#else
    USB2UART_Tx_DMA_Callback(1);
#endif
}

/************************************************************
	Bulk EP OUT handle functions (clear RX FIFO data, read them out and drop)
*************************************************************/
void USB_Acm_Rx_ClrFifo(uint8_t port)
{
    uint32_t nCount, index;
    uint8_t         data[HAL_USB_MAX_PACKET_SIZE_ENDPOINT_BULK_HIGH_SPEED];


    /* Check if data received */
    /* Check if data received, valid for double FIFO IP */

    for (index = 0; index < 2; index++) {
        nCount = hal_usb_get_rx_packet_length(g_UsbACM[port].rxpipe->byEP);

        if (nCount != 0) {
            /* Get the data from fifo */
            hal_usb_read_endpoint_fifo(g_UsbACM[port].rxpipe->byEP, nCount, data);

            /* Clear RxPktRdy */
            hal_usb_set_endpoint_rx_ready(g_UsbACM[port].rxpipe->byEP);

            /*Clear RX interrupt*/
            //hal_usb_clear_rx_endpoint_isr(g_UsbACM[port].rxpipe->byEP);
        }
    }
}


/************************************************************
	Interface initialization functions
*************************************************************/
uint8_t USB_Acm_Get_DataIf_Num(uint8_t port)
{
    uint8_t if_id;


    if (g_UsbACM[port].data_interface_id == 0xFF) {
        /* Get resource number and register to gUsbDevice */
        g_UsbACM[port].data_if_info = USB_Get_Interface_Number(&if_id);
        g_UsbACM[port].data_interface_id = if_id;
    }

    return g_UsbACM[port].data_interface_id;
}


/* Communication interface create function, prepare descriptor */
void USB_Acm_CommIf_Create(uint8_t port, void *ifname)
{
    uint8_t ep_id;
    uint8_t if_id;

    /* Get resource number and register to gUsbDevice */
    g_UsbACM[port].comm_if_info = USB_Get_Interface_Number(&if_id);
    g_UsbACM[port].comm_ep_intr_info = USB_Get_Intr_Ep(&ep_id);

    /* Record interface name and interface descriptor length */
    g_UsbACM[port].comm_if_info->interface_name_ptr = (char *)ifname;
    g_UsbACM[port].comm_if_info->ifdscr_size = USB_CDC_IF_LENGTH;
    g_UsbACM[port].comm_ep_intr_info->epdscr_size = USB_EPDSC_LENGTH;

    /* Related endpoint info and class specific command handler */
    g_UsbACM[port].comm_if_info->ep_info[0] = g_UsbACM[port].comm_ep_intr_info;
    if (port == 0) {
        g_UsbACM[port].comm_if_info->if_class_specific_hdlr = USB_Acm1_Ep0_Command;
    } else {
        g_UsbACM[port].comm_if_info->if_class_specific_hdlr = USB_Acm2_Ep0_Command;
    }

    memcpy(&(g_UsbACM[port].comm_if_info->ifdscr.stdif), cdcacm_com_if_dscr, USB_CDC_IF_LENGTH);
    memcpy(&(g_UsbACM[port].comm_ep_intr_info->epdesc.stdep), cdcacm_intr_dscr, USB_EPDSC_LENGTH);



    /* Standard interface descriptor */
    ((Usb_Cdc_If_Dscr *)g_UsbACM[port].comm_if_info->ifdscr.classif)->bInterfaceNumber = if_id;
    ((Usb_Cdc_If_Dscr *)g_UsbACM[port].comm_if_info->ifdscr.classif)->iInterface = USB_Get_String_Number((void *)g_UsbACM_Comm.acm_param->comm_interface_string);

    if (port == 0) {
        //hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_EP_TX, ep_id, USB_Acm1_IntrIn_Hdlr);
        g_UsbACM[port].comm_ep_intr_info->ep_reset = USB_Acm1_IntrIn_Reset;
    } else {
        //hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_EP_TX, ep_id, USB_Acm2_IntrIn_Hdlr);
        g_UsbACM[port].comm_ep_intr_info->ep_reset = USB_Acm2_IntrIn_Reset;
    }
#if 1
    {
        /* Construct IAD descriptor */
        g_UsbACM[port].iad_desc = USB_Get_IAD_Number();
        g_UsbACM[port].iad_desc->bLength = USB_IAD_LENGTH;
        g_UsbACM[port].iad_desc->bDescriptorType = USB_INTERFACE_ASSOCIATION;
        g_UsbACM[port].iad_desc->bInterfaceCount = USB_CDC_ACM_IF_NUMBER;

        if (if_id < USB_Acm_Get_DataIf_Num(port)) {
            /* Means first interface is communication interface */
            g_UsbACM[port].iad_desc->bFirstInterface = if_id;
            g_UsbACM[port].iad_desc->bFunctionClass = USB_ACM_COMM_INTERFACE_CLASS_CODE;
            g_UsbACM[port].iad_desc->bFunctionSubClass = USB_ACM_COMM_INTERFACE_SUBCLASS_CODE;
            g_UsbACM[port].iad_desc->bFunctionProtocol = USB_ACM_COMM_INTERFACE_PROTOCOL_CODE;
            g_UsbACM[port].iad_desc->iFunction = ((Usb_Cdc_If_Dscr *)g_UsbACM[port].comm_if_info->ifdscr.classif)->iInterface;
        } else {
            g_UsbACM[port].iad_desc->bFirstInterface = g_UsbACM[port].data_interface_id;
            g_UsbACM[port].iad_desc->bFunctionClass = USB_ACM_DATA_INTERFACE_CLASS_CODE;
            g_UsbACM[port].iad_desc->bFunctionSubClass = USB_ACM_DATA_INTERFACE_SUBCLASS_CODE;
            g_UsbACM[port].iad_desc->bFunctionProtocol = USB_ACM_DATA_INTERFACE_PROTOCOL_CODE;
            g_UsbACM[port].iad_desc->iFunction = g_UsbACM[port].data_if_info->ifdscr.stdif.iInterface;
        }
        if (g_UsbACM[port].iad_desc->iFunction == 0x00) {
            LOG_E(hal, "ASSERT\n");
        }
    }
#endif

    /* Union Functional Descriptor */
    ((Usb_Cdc_If_Dscr *)g_UsbACM[port].comm_if_info->ifdscr.classif)->bMasterInterface = if_id;
    ((Usb_Cdc_If_Dscr *)g_UsbACM[port].comm_if_info->ifdscr.classif)->bSlaveInterface0 = USB_Acm_Get_DataIf_Num(port); //g_UsbACM[port].data_interface_id;
    /* Call Management Descriptor */
    ((Usb_Cdc_If_Dscr *)g_UsbACM[port].comm_if_info->ifdscr.classif)->bDataInterface = USB_Acm_Get_DataIf_Num(port); //g_UsbACM[port].data_interface_id;

    /* Endpoint descriptor */
    g_UsbACM[port].comm_ep_intr_info->epdesc.stdep.bEndpointAddress = (USB_EP_DIR_IN | ep_id);	/*InPipe*/
    g_UsbACM[port].comm_ep_intr_info->epdesc.stdep.bInterval = 3;
    g_UsbACM[port].comm_ep_intr_info->ep_status.epin_status.byEP = ep_id;
    g_UsbACM[port].comm_ep_intr_info->ep_status.epin_status.nBytesLeft = USB_EP_NODATA;

    g_UsbACM[port].intr_pkt.bmRequestType = USB_CMD_CLASSIFIN;
    g_UsbACM[port].intr_pkt.bRequest = USB_ACM_NOTIF_SERIAL_STATE;
    g_UsbACM[port].intr_pkt.wValue = 0;
    g_UsbACM[port].intr_pkt.wIndex = if_id;
    g_UsbACM[port].intr_pkt.wLength = 2;
}


/* Communication interface reset function, should enable EP, but we do not use this interrupt EP so not enable it */
void USB_Acm_CommIf_Reset(uint8_t port)
{
    g_UsbACM[port].intrpipe = &g_UsbACM[port].comm_ep_intr_info->ep_status.epin_status;
}


void USB_Acm_CommIf_Enable(uint8_t port)
{
    /* Non-DMA */
    hal_usb_enable_tx_endpoint(g_UsbACM[port].intrpipe->byEP, HAL_USB_EP_TRANSFER_INTR, HAL_USB_EP_USE_NO_DMA, true);
}

/* Communication interface speed reset function, enable EP's speed-specific descriptor */
void USB_Acm_CommIf_Speed_Reset(uint8_t port, bool b_other_speed)
{

    g_UsbACM[port].comm_ep_intr_info->epdesc.stdep.wMaxPacketSize[0] = HAL_USB_MAX_PACKET_SIZE_ENDPOINT_INTERRUPT_FULL_SPEED & 0xff;
    g_UsbACM[port].comm_ep_intr_info->epdesc.stdep.wMaxPacketSize[1] = 0x00;
}




/* Data interface create function, prepare descriptor */
void USB_Acm_DataIf_Create(uint8_t port, void *ifname)
{
    uint8_t ep_tx_id;
    uint8_t ep_rx_id;
    uint8_t if_id;

    /* Get resource number and register to gUsbDevice */
    if (g_UsbACM[port].data_interface_id == 0xFF) { //check has already register or not
        g_UsbACM[port].data_if_info = USB_Get_Interface_Number(&if_id);
        g_UsbACM[port].data_interface_id = if_id;
    }

    g_UsbACM[port].data_ep_in_info = USB_Get_Bulk_Tx_Ep(&ep_tx_id);
    g_UsbACM[port].data_ep_out_info = USB_Get_Bulk_Rx_Ep(&ep_rx_id);

    /* Record interface name and interface descriptor length */
    g_UsbACM[port].data_if_info->interface_name_ptr = (char *)ifname;
    g_UsbACM[port].data_if_info->ifdscr_size = USB_IFDSC_LENGTH;
    g_UsbACM[port].data_ep_in_info->epdscr_size = USB_EPDSC_LENGTH;
    g_UsbACM[port].data_ep_out_info->epdscr_size = USB_EPDSC_LENGTH;

    /* Related endpoint info and class specific command handler*/
    g_UsbACM[port].data_if_info->ep_info[0] = g_UsbACM[port].data_ep_in_info;
    g_UsbACM[port].data_if_info->ep_info[1] = g_UsbACM[port].data_ep_out_info;
    g_UsbACM[port].data_if_info->if_class_specific_hdlr = (usb_class_specific_handler_ptr)USB_Acm_Ep0_Command; //align with Bootrom comm_if

    memcpy(&(g_UsbACM[port].data_if_info->ifdscr.stdif), cdcacm_data_if_dscr, USB_IFDSC_LENGTH);
    memcpy(&(g_UsbACM[port].data_ep_in_info->epdesc.stdep), cdcacm_ep_in_dscr, USB_EPDSC_LENGTH);
    memcpy(&(g_UsbACM[port].data_ep_out_info->epdesc.stdep), cdcacm_ep_out_dscr, USB_EPDSC_LENGTH);

    /* Standard interface descriptor */
    g_UsbACM[port].data_if_info->ifdscr.stdif.bInterfaceNumber = g_UsbACM[port].data_interface_id;
    g_UsbACM[port].data_if_info->ifdscr.stdif.iInterface = USB_Get_String_Number((void *)g_UsbACM_Comm.acm_param->data_interface_string);
    if (port == 0) {
        g_UsbACM[port].data_ep_in_info->ep_reset = USB_Acm1_BulkIn_Reset;
        g_UsbACM[port].data_ep_out_info->ep_reset = USB_Acm1_BulkOut_Reset;
    } else {
        g_UsbACM[port].data_ep_in_info->ep_reset = USB_Acm2_BulkIn_Reset;
        g_UsbACM[port].data_ep_out_info->ep_reset = USB_Acm2_BulkOut_Reset;
    }


    /* TX Endpoint handler */
    hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_EP_TX, ep_tx_id, NULL);

    /* RX Endpoint handler */
    /* DMA */
    if (port == 0) {
        hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_EP_RX, ep_rx_id, USB_Acm1_BulkOut_Hdr);
    } else {
        hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_EP_RX, ep_rx_id, USB_Acm2_BulkOut_Hdr);
    }

    /* Endpoint descriptor */
    g_UsbACM[port].data_ep_in_info->epdesc.stdep.bEndpointAddress = (USB_EP_DIR_IN | ep_tx_id); /*InPipe*/
    g_UsbACM[port].data_ep_in_info->ep_status.epin_status.byEP = ep_tx_id;
    g_UsbACM[port].data_ep_in_info->ep_status.epin_status.nBytesLeft = USB_EP_NODATA;
    g_UsbACM[port].data_ep_out_info->epdesc.stdep.bEndpointAddress = (USB_EP_DIR_OUT | ep_rx_id); /*OutPipe*/
    g_UsbACM[port].data_ep_out_info->ep_status.epout_status.byEP = ep_rx_id;

    hal_usb_get_dma_channel(ep_tx_id, 0, HAL_USB_EP_DIRECTION_TX, false);
}

/* Data interface reset function, enable EP */
void USB_Acm_DataIf_Reset(uint8_t port)
{
    g_UsbACM[port].txpipe = &g_UsbACM[port].data_ep_in_info->ep_status.epin_status;
    g_UsbACM[port].rxpipe = &g_UsbACM[port].data_ep_out_info->ep_status.epout_status;
}


void USB_Acm_DataIf_Enable(uint8_t port)
{
    /* DMA */
    hal_usb_enable_tx_endpoint(g_UsbACM[port].txpipe->byEP, HAL_USB_EP_TRANSFER_BULK, HAL_USB_EP_USE_ONLY_DMA, true);

    /*Non-DMA*/
    hal_usb_enable_rx_endpoint(g_UsbACM[port].rxpipe->byEP, HAL_USB_EP_TRANSFER_BULK, HAL_USB_EP_USE_NO_DMA, true);
}




/* data interface speed reset function, enable EP's speed-specific descriptor */
void USB_Acm_DataIf_Speed_Reset(uint8_t port, bool b_other_speed)
{
    uint32_t	temp_max_size;
    temp_max_size = USB_Speed_Reset_Packet_Size((bool)b_other_speed);

    g_UsbACM[port].data_ep_in_info->epdesc.stdep.wMaxPacketSize[0] = temp_max_size & 0xff;
    g_UsbACM[port].data_ep_in_info->epdesc.stdep.wMaxPacketSize[1] = (temp_max_size >> 8) & 0xff;
    g_UsbACM[port].data_ep_out_info->epdesc.stdep.wMaxPacketSize[0] = temp_max_size & 0xff;
    g_UsbACM[port].data_ep_out_info->epdesc.stdep.wMaxPacketSize[1] = (temp_max_size >> 8) & 0xff;
}

/************************************************************
	global variable g_UsbACM initialize and release functions
*************************************************************/
void USB_Acm_Default_Setting(uint8_t port)
{
    g_UsbACM[port].txpipe = NULL;
    g_UsbACM[port].rxpipe = NULL;
    g_UsbACM[port].intrpipe = NULL;
    g_UsbACM[port].data_interface_id = 0xFF;
    g_UsbACM[port].break_detect = false;
    g_UsbACM[port].break_number = 0;
    g_UsbACM[port].send_Txilm = false;
    g_UsbACM[port].send_Rxilm = true;
    g_UsbACM[port].send_UARTilm = false;
    g_UsbACM[port].intr_state = ACM_INTR_IDLE;
    g_UsbACM[port].setup_dma = false;
    g_UsbACM[port].line_state = 0;
    if (g_UsbACM[port].put_start == true) {
        g_UsbACM[port].dmaf_setmember |= 0x04;
    }
    g_UsbACM[port].dma_txcb_just_done = false;
}

/* Initialize global variable g_UsbACM */
void USB_Init_Acm_Status(void)
{
    USB_Acm_Default_Setting(0);
    USB_Acm_Default_Setting(1);
}


/* Release global variable g_UsbACM */
void USB_Release_Acm_Status(void)
{
    if (g_UsbACM[0].txpipe != NULL) {
        hal_usb_release_dma_channel(g_UsbACM[0].txpipe->byEP, 0, HAL_USB_EP_DIRECTION_TX, false);
    }
    if (g_UsbACM[1].txpipe != NULL) {
        hal_usb_release_dma_channel(g_UsbACM[1].txpipe->byEP, 0, HAL_USB_EP_DIRECTION_TX, false);
    }

    USB_Acm_Default_Setting(0);
    USB_Acm_Default_Setting(1);

    USB_Free_Memory();

    g_UsbACM[0].config_send_Txilm = false;
    g_UsbACM[1].config_send_Txilm = false;
}

/************************************************************
	EP0 handle functions
************************************************************/
extern uint32_t hal_usb_ep0_pkt_len(void);

/* Set USB request line coding to UART DCB structure */
void USB_Acm_Ep0_SetLineCoding(uint8_t port, void *data)
{
    uint32_t nCount;
    bool stall = false;

    /* Read setting and set to UART structure */
    nCount = hal_usb_ep0_pkt_len();
    if (nCount == 7) {
        hal_usb_read_endpoint_fifo(0, nCount, &g_UsbACM[port].line_coding);
    } else {
        stall = true;
    }

    //USB_Set_Endpoint0_State(USB_EP0_RX_STATUS);
    gUsbDevice.ep0_state = USB_EP0_RX_STATUS;
    hal_usb_update_endpoint_0_state(HAL_USB_EP0_DRV_STATE_READ_END, stall, true);
}


/* Parse class specific request */
void USB_Acm_Ep0_Command(uint8_t port, Usb_Ep0_Status *pep0state, Usb_Command *pcmd)
{
    bool  bError = false;

    switch (pcmd->bRequest) {
        case USB_ACM_SET_LINE_CODING:
            /* register handler to handle the get data*/
            //USB_Register_EP0_RxHdlr(USB_Acm_Ep0_SetLineCoding);
            gUsbDevice.ep0_rx_handler = (usb_ep0_rx_ptr)USB_Acm_Ep0_SetLineCoding;
            //USB_Set_Endpoint0_State(USB_EP0_RX);
            gUsbDevice.ep0_state = USB_EP0_RX;
            break;
        case USB_ACM_GET_LINE_CODING:
            /* tell host the current setting */
            USB_Generate_EP0Data(pep0state, pcmd, &g_UsbACM[port].line_coding, 7);
            break;
        case USB_ACM_SET_CONTROL_LINE_STATE:
            g_UsbACM[port].line_state = pcmd->wValue;
            /* do not need to do anything */
            break;
        case USB_ACM_SEND_BREAK:
            /* Break behavior */
            switch (pcmd->wValue) {
                case 0xffff:
                    g_UsbACM[port].break_detect = true;
                    break;
                case 0x0:
                    g_UsbACM[port].break_detect = false;
                    g_UsbACM[port].break_number++;
                    break;
                default:
                    g_UsbACM[port].break_detect = true;
                    break;
            }
            break;

        case USB_ACM_SEND_ENCAPSULATED_COMMAND:
        case USB_ACM_GET_ENCAPSULATED_RESPONSE:
        case USB_ACM_SET_COMM_FEATURE:
        case USB_ACM_GET_COMM_FEATURE:
        case USB_ACM_CLEAR_COMM_FEATURE:
        default:
            bError = true;
            break;
    }

    /* Stall command if an error occured */
    USB_EP0_Command_Hdlr(bError);
}


/*
    Used to retreive exception log, all interrupts are disabled
    Note that this is special for sending exception log since interrupt is disabled when exception occurs
    It must not be used in normal time
*/
void USB2UART_Polling_Recv_Data(uint32_t port)
{
    uint32_t usb_port = USB_PORT[port];

    /* Avoid sending ilm to UART owner*/
    g_UsbACM[usb_port].send_Txilm = false;
    g_UsbACM[usb_port].send_Rxilm = false;

    //hal_usb_polling_rx_data();
}


/************************************************************
	Bulk EP OUT handle functions
*************************************************************/
/* EP Bulk Out interrupt handler, called by EP interrupt */
// for echo test, dma
//ATTR_RWDATA_IN_NONCACHED_RAM static uint8_t  data[10];

void USB_Acm_BulkOut_Hdr(uint8_t port)
{
#if 0
    uint32_t RoomLeft;
    uint32_t nCount;
    uint32_t index;
    //uint8_t data[512];
    int32_t remain;
    BUFFER_INFO *rx_buf_info = &(USB2UARTPort[port].Rx_Buffer);

    nCount = hal_usb_get_rx_packet_length(g_UsbACM[port].rxpipe->byEP);
    if (nCount != 0) {
        hal_usb_read_endpoint_fifo(g_UsbACM[port].rxpipe->byEP, nCount, BuffWrite(rx_buf_info));
        BWrite(rx_buf_info) += nCount;


        //hal_usb_read_endpoint_fifo(g_UsbACM[port].rxpipe->byEP, nCount, data);
        hal_usb_set_endpoint_rx_ready(g_UsbACM[port].rxpipe->byEP);

        //USB2UARTPort[port].rx_cb(SERIAL_PORT_EVENT_READY_TO_READ, NULL);
    } else {
        /* Clear RxPktRdy, drop this packet */
        hal_usb_set_endpoint_rx_ready(g_UsbACM[port].rxpipe->byEP);
    }

#else
    uint32_t usb_port = port;//USB_PORT[port];
    uint32_t RoomLeft;
    uint32_t nCount;
    uint32_t index;
    uint8_t data[4];
    int32_t remain;
    BUFFER_INFO *rx_buf_info = &(USB2UARTPort[usb_port].Rx_Buffer);
    uint32_t max_bulk_pkt_size;

    if (USB2UARTPort[usb_port].initialized == false) {
        return;
    }

    /* Check current packet's data length */
    //nCount = USB_EP_Rx_Pkt_Len(g_UsbACM[usb_port].rxpipe->byEP);
    nCount = hal_usb_get_rx_packet_length(g_UsbACM[usb_port].rxpipe->byEP);

    /* No UART owner, drop the data directly */
    if (nCount != 0) {

        max_bulk_pkt_size = USB_Bulk_Max_Packet_Size();

        RoomLeft = USB2UART_GetBuffRoomLeft(rx_buf_info);
        if ((nCount > max_bulk_pkt_size) || (RoomLeft < nCount)) {
            //EXT_ASSERT(0, nCount, max_bulk_pkt_size, RoomLeft);
            LOG_E(hal, "ASSERT, USB_Acm_BulkOut_Hdr\n");
        }

        /* Update roomleft parameter*/
        RoomLeft -= nCount;

        if (RoomLeft < max_bulk_pkt_size) {
            /* RX buffer is less than one max packet size, so mask the IRQ until UART owner get bytes,
            	and than enough room to put a complete packet */
            //USB_Set_UnMask_Irq(false);
            USB_Mask_COM_Intr(port);
        }

        if (nCount > 4) {
            remain = (int32_t)((BWrite(rx_buf_info) + nCount) - BLength(rx_buf_info));

            if (remain < 0) {
                //USB_EPFIFORead(g_UsbACM[usb_port].rxpipe->byEP, nCount, BuffWrite(rx_buf_info), false);
                hal_usb_read_endpoint_fifo(g_UsbACM[usb_port].rxpipe->byEP, nCount, BuffWrite(rx_buf_info));
                BWrite(rx_buf_info) += nCount;
            } else {
                hal_usb_read_endpoint_fifo(g_UsbACM[usb_port].rxpipe->byEP, (nCount - remain), BuffWrite(rx_buf_info));
                hal_usb_read_endpoint_fifo(g_UsbACM[usb_port].rxpipe->byEP, remain, BStartAddr(rx_buf_info));
                BWrite(rx_buf_info) = remain;
            }

        } else {
            hal_usb_read_endpoint_fifo(g_UsbACM[usb_port].rxpipe->byEP, nCount, data);
            /* Push all the received data into rx ring buffer */
            for (index = 0; index < nCount; index++) {
                Buf_Push(rx_buf_info, data[index]);
            }
        }
        /* RX complete callback */
        USB2UARTPort[usb_port].rx_cb(port);

        /* Clear RxPktRdy */
        hal_usb_set_endpoint_rx_ready(g_UsbACM[usb_port].rxpipe->byEP);
    } else { /* if((USB2UARTPort[usb_port].ownerid != MOD_DRV_HISR)||(nCount != 0)) */
        /* Clear RxPktRdy, drop this packet */
        hal_usb_set_endpoint_rx_ready(g_UsbACM[usb_port].rxpipe->byEP);
    }
#endif

#if 0
    //DMA echo test
    LOG_E(hal, "USB_Acm_BulkOut_Hdr: write %d, data, 0x%x\n", nCount, data[0]);
    hal_usb_stop_dma_channel(g_UsbACM[port].txpipe->byEP, HAL_USB_EP_DIRECTION_TX);
    hal_usb_start_dma_channel(g_UsbACM[port].txpipe->byEP, HAL_USB_EP_DIRECTION_TX, HAL_USB_EP_TRANSFER_BULK, data, nCount, NULL, true, HAL_USB_DMA1_TYPE);
#endif
}


/* EP Bulk In reset handler */
void USB_Acm_BulkIn_Reset(uint8_t port)
{
    g_UsbACM[port].txpipe = &g_UsbACM[port].data_ep_in_info->ep_status.epin_status;
}


/* EP Bulk Out reset handler */
void USB_Acm_BulkOut_Reset(uint8_t port)
{
    g_UsbACM[port].rxpipe = &g_UsbACM[port].data_ep_out_info->ep_status.epout_status;
}

#if 0
/* When PC return ACK, handle ACM intr pipe state machine */
void USB_Acm_IntrIn_Hdlr(uint8_t port)
{
    uint32_t len;
    uint32_t max_intr_pkt_size;

    if (gUsbDevice.nDevState != DEVSTATE_CONFIG) {
        /* USB reset */
        //USB_Dbg_Trace(USB_ACM_INTERRUPT_RESET, 0, 0);
        g_UsbACM[port].intr_state = ACM_INTR_IDLE;
        return;
    }

    max_intr_pkt_size = USB_Intr_Max_Packet_Size();

    switch (g_UsbACM[port].intr_state) {
        case ACM_INTR_SEND_ONE_PKT:
            g_UsbACM[port].intr_state = ACM_INTR_SEND_LAST_PKT;

			len = ((uint32_t)sizeof(UsbAcm_Intr_Pkt)) - max_intr_pkt_size;

			/* send last short packet */
            hal_usb_write_endpoint_fifo(g_UsbACM[port].intrpipe->byEP, len, (uint8_t *)(uint32_t)&g_UsbACM[port].intr_pkt + max_intr_pkt_size);
            hal_usb_set_endpoint_tx_ready(g_UsbACM[port].intrpipe->byEP);
            break;

        case ACM_INTR_SEND_LAST_PKT:
            g_UsbACM[port].intr_state = ACM_INTR_IDLE;

            /* Send another intr pkt */
            if (g_UsbACM[port].is_intr_pending_pkt == true) {
                g_UsbACM[port].is_intr_pending_pkt = false;
            }
            break;

        case ACM_INTR_IDLE:
        default:
            break;
    }
}
#endif

void USB_Acm_IntrIn_Reset(uint8_t port)
{
    g_UsbACM[port].intr_state = ACM_INTR_IDLE;
}

#endif //MTK_USB_DEMO_ENABLED
