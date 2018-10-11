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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "memory_attribute.h"
#include "hal_usb.h"
#include "usb_comm.h"
#include "usb.h"
#include "usb_resource.h"
#include "usbacm_drv.h"


/* Global variables */
UsbAcm_Struct g_UsbACM;
UsbAcm_Common_Struct g_UsbACM_Comm;


/* Static functions */
static uint8_t USB_Acm_Get_DataIf_Num(void);
void USB_Acm_CommIf_Create(void *ifname);
void USB_Acm_CommIf_Reset(void);
void USB_Acm_CommIf_Enable(void);
void USB_Acm_CommIf_Speed_Reset(bool b_other_speed);
void USB_Acm_DataIf_Create(void *ifname);
void USB_Acm_DataIf_Reset(void);
void USB_Acm_DataIf_Enable(void);
void USB_Acm_DataIf_Speed_Reset(bool b_other_speed);
void USB_Acm_Break_Timeout(void *parameter);
void USB_Acm_Ring_Buffer_Timeout(void *parameter);
void USB_Acm_Ep0_SetLineCoding(void *data);
void USB_Acm_Ep0_Command(Usb_Ep0_Status* pep0state, Usb_Command* pcmd);
void USB_Acm_BulkOut_Hdr(void);
void USB_Acm_BulkIn_Reset(void);
void USB_Acm_BulkOut_Reset(void);
void USB_Acm_IntrIn_Hdlr(void);
void USB_Acm_IntrIn_Reset(void);

static const uint8_t cdcacm_com_if_dscr[]=
{
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

const uint8_t cdcacm_intr_dscr[]=
{
	USB_EPDSC_LENGTH,	//bLength;
	USB_ENDPOINT,	//bDescriptorType;
	0x00,	//bEndpointAddress;
	USB_EP_INTR,	//bmAttributes;
	HAL_USB_MAX_PACKET_SIZE_ENDPOINT_INTERRUPT_HIGH_SPEED&0xff,	//wMaxPacketSize[2];	
	0x00,	//wMaxPacketSize[2];	
	0x03	//bInterval;
};



const uint8_t cdcacm_data_if_dscr[]=
{
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


const uint8_t cdcacm_ep_in_dscr[]=
{
	USB_EPDSC_LENGTH,	//bLength;
	USB_ENDPOINT,	//bDescriptorType;
	0x00,	//bEndpointAddress;
	USB_EP_BULK,	//bmAttributes;
	0x40,	//wMaxPacketSize[2];	
	0x00,	//wMaxPacketSize[2];	
	0x00	//bInterval;
};

const uint8_t cdcacm_ep_out_dscr[]=
{
	USB_EPDSC_LENGTH,	//bLength;
	USB_ENDPOINT,	//bDescriptorType;
	0x00,	//bEndpointAddress;
	USB_EP_BULK,	//bmAttributes;
	0x40,	//wMaxPacketSize[2];	
	0x00,	//wMaxPacketSize[2];	
	0x00	//bInterval;
};


/************************************************************
	Bulk EP OUT handle functions (clear RX FIFO data, read them out and drop)
*************************************************************/
void USB_Acm_Rx_ClrFifo(void)
{
	uint32_t nCount,index;
	uint8_t         data[HAL_USB_MAX_PACKET_SIZE_ENDPOINT_BULK_HIGH_SPEED];


	/* Check if data received */
	/* Check if data received, valid for double FIFO IP */

	for (index = 0; index<2; index++)
	{
		nCount = hal_usb_get_rx_packet_length(g_UsbACM.rxpipe->byEP);

		if(nCount != 0)
		{
			/* Get the data from fifo */
			hal_usb_read_endpoint_fifo(g_UsbACM.rxpipe->byEP, nCount, data);

			/* Clear RxPktRdy */
			hal_usb_set_endpoint_rx_ready(g_UsbACM.rxpipe->byEP);
			
			/*Clear RX interrupt*/
			//hal_usb_clear_rx_endpoint_isr(g_UsbACM.rxpipe->byEP);
		}
	}
}


/************************************************************
	Interface initialization functions
*************************************************************/
uint8_t USB_Acm_Get_DataIf_Num(void)
{
	uint8_t if_id;


	if(g_UsbACM.data_interface_id == 0xFF)
	{
		/* Get resource number and register to gUsbDevice */
		g_UsbACM.data_if_info = USB_Get_Interface_Number(&if_id);
		g_UsbACM.data_interface_id = if_id;
	}

	return g_UsbACM.data_interface_id;
}


/* Communication interface create function, prepare descriptor */
void USB_Acm_CommIf_Create(void *ifname)
{
	uint8_t ep_id;
	uint8_t if_id;

	/* Get resource number and register to gUsbDevice */
	g_UsbACM.comm_if_info = USB_Get_Interface_Number(&if_id);
	g_UsbACM.comm_ep_intr_info = USB_Get_Intr_Ep(&ep_id);

	/* Record interface name and interface descriptor length */
	g_UsbACM.comm_if_info->interface_name_ptr = (kal_char *)ifname;
	g_UsbACM.comm_if_info->ifdscr_size = USB_CDC_IF_LENGTH;
	g_UsbACM.comm_ep_intr_info->epdscr_size = USB_EPDSC_LENGTH;

	/* Related endpoint info and class specific command handler */
	g_UsbACM.comm_if_info->ep_info[0] = g_UsbACM.comm_ep_intr_info;
	g_UsbACM.comm_if_info->if_class_specific_hdlr = USB_Acm_Ep0_Command;

	memcpy(&(g_UsbACM.comm_if_info->ifdscr.stdif), cdcacm_com_if_dscr, USB_CDC_IF_LENGTH);
	memcpy(&(g_UsbACM.comm_ep_intr_info->epdesc.stdep), cdcacm_intr_dscr, USB_EPDSC_LENGTH);



	/* Standard interface descriptor */
	((Usb_Cdc_If_Dscr*)g_UsbACM.comm_if_info->ifdscr.classif)->bInterfaceNumber = if_id;


	((Usb_Cdc_If_Dscr*)g_UsbACM.comm_if_info->ifdscr.classif)->iInterface =USB_Get_String_Number((void *)g_UsbACM_Comm.acm_param->comm_interface_string);
	/* Endpoint handler */
	hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_EP_TX, ep_id, USB_Acm_IntrIn_Hdlr);
	g_UsbACM.comm_ep_intr_info->ep_reset = USB_Acm_IntrIn_Reset;

	/* Union Functional Descriptor */
	((Usb_Cdc_If_Dscr*)g_UsbACM.comm_if_info->ifdscr.classif)->bMasterInterface = if_id;
	((Usb_Cdc_If_Dscr*)g_UsbACM.comm_if_info->ifdscr.classif)->bSlaveInterface0 = USB_Acm_Get_DataIf_Num();//g_UsbACM.data_interface_id;
	/* Call Management Descriptor */
	((Usb_Cdc_If_Dscr*)g_UsbACM.comm_if_info->ifdscr.classif)->bDataInterface = USB_Acm_Get_DataIf_Num();//g_UsbACM.data_interface_id;

	/* Endpoint descriptor */
	g_UsbACM.comm_ep_intr_info->epdesc.stdep.bEndpointAddress = (USB_EP_DIR_IN|ep_id);	/*InPipe*/
	g_UsbACM.comm_ep_intr_info->epdesc.stdep.bInterval = 3;
	g_UsbACM.comm_ep_intr_info->ep_status.epin_status.byEP = ep_id;
	g_UsbACM.comm_ep_intr_info->ep_status.epin_status.nBytesLeft = USB_EP_NODATA;

	g_UsbACM.intr_pkt.bmRequestType = USB_CMD_CLASSIFIN;
	g_UsbACM.intr_pkt.bRequest = USB_ACM_NOTIF_SERIAL_STATE;
	g_UsbACM.intr_pkt.wValue = 0;
	g_UsbACM.intr_pkt.wIndex = if_id;
	g_UsbACM.intr_pkt.wLength = 2;
}


/* Communication interface reset function, should enable EP, but we do not use this interrupt EP so not enable it */
void USB_Acm_CommIf_Reset(void)
{
	g_UsbACM.intrpipe = &g_UsbACM.comm_ep_intr_info->ep_status.epin_status;
}


void USB_Acm_CommIf_Enable(void)
{
	/* Non-DMA */
	hal_usb_enable_tx_endpoint(g_UsbACM.intrpipe->byEP, HAL_USB_EP_TRANSFER_INTR, HAL_USB_EP_USE_NO_DMA, true);
}

/* Communication interface speed reset function, enable EP's speed-specific descriptor */
void USB_Acm_CommIf_Speed_Reset(bool b_other_speed)
{

	g_UsbACM.comm_ep_intr_info->epdesc.stdep.wMaxPacketSize[0] = HAL_USB_MAX_PACKET_SIZE_ENDPOINT_INTERRUPT_FULL_SPEED&0xff;
	g_UsbACM.comm_ep_intr_info->epdesc.stdep.wMaxPacketSize[1] = 0x00;
}




/* Data interface create function, prepare descriptor */
void USB_Acm_DataIf_Create(void *ifname)
{
	uint8_t ep_tx_id;
	uint8_t ep_rx_id;
	uint8_t if_id;

	/* Get resource number and register to gUsbDevice */
	if(g_UsbACM.data_interface_id == 0xFF)  //check has already register or not
	{
		g_UsbACM.data_if_info = USB_Get_Interface_Number(&if_id);
		g_UsbACM.data_interface_id = if_id;
	}

	g_UsbACM.data_ep_in_info = USB_Get_Bulk_Tx_Ep(&ep_tx_id);
	g_UsbACM.data_ep_out_info = USB_Get_Bulk_Rx_Ep(&ep_rx_id);

	/* Record interface name and interface descriptor length */
	g_UsbACM.data_if_info->interface_name_ptr = (kal_char *)ifname;
	g_UsbACM.data_if_info->ifdscr_size = USB_IFDSC_LENGTH;
	g_UsbACM.data_ep_in_info->epdscr_size = USB_EPDSC_LENGTH;
	g_UsbACM.data_ep_out_info->epdscr_size = USB_EPDSC_LENGTH;

	/* Related endpoint info and class specific command handler*/
	g_UsbACM.data_if_info->ep_info[0] = g_UsbACM.data_ep_in_info;
	g_UsbACM.data_if_info->ep_info[1] = g_UsbACM.data_ep_out_info;
	g_UsbACM.data_if_info->if_class_specific_hdlr = USB_Acm_Ep0_Command; //align with Bootrom comm_if

	memcpy(&(g_UsbACM.data_if_info->ifdscr.stdif), cdcacm_data_if_dscr, USB_IFDSC_LENGTH);
	memcpy(&(g_UsbACM.data_ep_in_info->epdesc.stdep), cdcacm_ep_in_dscr, USB_EPDSC_LENGTH);
	memcpy(&(g_UsbACM.data_ep_out_info->epdesc.stdep), cdcacm_ep_out_dscr, USB_EPDSC_LENGTH);

	/* Standard interface descriptor */
	g_UsbACM.data_if_info->ifdscr.stdif.bInterfaceNumber = g_UsbACM.data_interface_id;
	g_UsbACM.data_if_info->ifdscr.stdif.iInterface = USB_Get_String_Number((void *)g_UsbACM_Comm.acm_param->data_interface_string);
	g_UsbACM.data_ep_in_info->ep_reset = USB_Acm_BulkIn_Reset;
	g_UsbACM.data_ep_out_info->ep_reset = USB_Acm_BulkOut_Reset;


	/* TX Endpoint handler */
	hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_EP_TX, ep_tx_id, NULL);

	/* RX Endpoint handler */
	hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_EP_RX, ep_rx_id, USB_Acm_BulkOut_Hdr);

	/* Endpoint descriptor */
	g_UsbACM.data_ep_in_info->epdesc.stdep.bEndpointAddress = (USB_EP_DIR_IN|ep_tx_id);   /*InPipe*/

	g_UsbACM.data_ep_in_info->ep_status.epin_status.byEP = ep_tx_id;
	g_UsbACM.data_ep_in_info->ep_status.epin_status.nBytesLeft = USB_EP_NODATA;
	g_UsbACM.data_ep_out_info->epdesc.stdep.bEndpointAddress = (USB_EP_DIR_OUT|ep_rx_id);   /*OutPipe*/
	g_UsbACM.data_ep_out_info->ep_status.epout_status.byEP = ep_rx_id;

	hal_usb_get_dma_channel(ep_tx_id, 0, HAL_USB_EP_DIRECTION_TX, false);
}

/* Data interface reset function, enable EP */
void USB_Acm_DataIf_Reset(void)
{
	g_UsbACM.txpipe = &g_UsbACM.data_ep_in_info->ep_status.epin_status;
	g_UsbACM.rxpipe = &g_UsbACM.data_ep_out_info->ep_status.epout_status;
}


void USB_Acm_DataIf_Enable(void)
{
	/* DMA */
	hal_usb_enable_tx_endpoint(g_UsbACM.txpipe->byEP, HAL_USB_EP_TRANSFER_BULK, HAL_USB_EP_USE_ONLY_DMA, true);

	/*Non-DMA*/
	hal_usb_enable_rx_endpoint(g_UsbACM.rxpipe->byEP, HAL_USB_EP_TRANSFER_BULK, HAL_USB_EP_USE_NO_DMA, true);
}




/* data interface speed reset function, enable EP's speed-specific descriptor */
void USB_Acm_DataIf_Speed_Reset(bool b_other_speed)
{
	uint32_t	temp_max_size;
	temp_max_size = USB_Speed_Reset_Packet_Size(b_other_speed);	
	
	g_UsbACM.data_ep_in_info->epdesc.stdep.wMaxPacketSize[0] = temp_max_size&0xff;
	g_UsbACM.data_ep_in_info->epdesc.stdep.wMaxPacketSize[1] = (temp_max_size>>8)&0xff;
	g_UsbACM.data_ep_out_info->epdesc.stdep.wMaxPacketSize[0] = temp_max_size&0xff;
	g_UsbACM.data_ep_out_info->epdesc.stdep.wMaxPacketSize[1] = (temp_max_size>>8)&0xff;
}

/************************************************************
	global variable g_UsbACM initialize and release functions
*************************************************************/
void USB_Acm_Default_Setting(void)
{
	g_UsbACM.txpipe = NULL;
	g_UsbACM.rxpipe = NULL;
	g_UsbACM.intrpipe = NULL;
	g_UsbACM.data_interface_id = 0xFF;
	g_UsbACM.break_detect = false;
	g_UsbACM.break_number = 0;
	g_UsbACM.send_Txilm = false;
	g_UsbACM.send_Rxilm = true;
	g_UsbACM.send_UARTilm = false;
	g_UsbACM.ring_buffer_timer_counting = false;
	g_UsbACM.intr_state = ACM_INTR_IDLE;
	g_UsbACM.setup_dma = false;
	g_UsbACM.line_state = 0;
	if(g_UsbACM.put_start == true)
	{
	   g_UsbACM.dmaf_setmember |= 0x04;
	}	
	g_UsbACM.dma_txcb_just_done = false;
}

/* Initialize global variable g_UsbACM */
void USB_Init_Acm_Status(void)
{
		USB_Acm_Default_Setting();
}


/* Release global variable g_UsbACM */
void USB_Release_Acm_Status(void)
{
	if(g_UsbACM.txpipe != NULL)
		hal_usb_release_dma_channel(g_UsbACM.txpipe->byEP, 0, HAL_USB_EP_DIRECTION_TX, false);

	USB_Acm_Default_Setting();
	g_UsbACM.config_send_Txilm = false;
}

/************************************************************
	EP0 handle functions
************************************************************/
extern uint32_t hal_usb_ep0_pkt_len(void);

/* Set USB request line coding to UART DCB structure */
void USB_Acm_Ep0_SetLineCoding(void *data)
{
	uint32_t nCount;
	bool stall = false;

	/* Read setting and set to UART structure */
	nCount = hal_usb_ep0_pkt_len();
	if(nCount == 7)
	{
		hal_usb_read_endpoint_fifo(0, nCount, &g_UsbACM.line_coding);
	}
	else
	{
		stall = true;
	}

	//USB_Set_Endpoint0_State(USB_EP0_RX_STATUS);
	gUsbDevice.ep0_state = USB_EP0_RX_STATUS;
	hal_usb_update_endpoint_0_state(HAL_USB_EP0_DRV_STATE_READ_END, stall, true);
}


/* Parse class specific request */
void USB_Acm_Ep0_Command(Usb_Ep0_Status* pep0state, Usb_Command* pcmd)
{
	bool  bError = false;

	switch (pcmd->bRequest)
	{
	case USB_ACM_SET_LINE_CODING:
		/* register handler to handle the get data*/
		//USB_Register_EP0_RxHdlr(USB_Acm_Ep0_SetLineCoding);
		gUsbDevice.ep0_rx_handler = USB_Acm_Ep0_SetLineCoding;
		//USB_Set_Endpoint0_State(USB_EP0_RX);
		gUsbDevice.ep0_state = USB_EP0_RX;
		break;
	case USB_ACM_GET_LINE_CODING:
		/* tell host the current setting */
		USB_Generate_EP0Data(pep0state, pcmd, &g_UsbACM.line_coding, 7);
		break;
	case USB_ACM_SET_CONTROL_LINE_STATE:
		g_UsbACM.line_state = pcmd->wValue;
		/* do not need to do anything */
		break;
	case USB_ACM_SEND_BREAK:
		/* Break behavior */
		switch(pcmd->wValue)
		{
		case 0xffff:
			g_UsbACM.break_detect = true;
			break;
		case 0x0:
			g_UsbACM.break_detect = false;
			g_UsbACM.break_number++;
			break;
		default:
			g_UsbACM.break_detect = true;
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




/************************************************************
	Bulk EP OUT handle functions
*************************************************************/
/* EP Bulk Out interrupt handler, called by EP interrupt */
ATTR_RWDATA_IN_NONCACHED_RAM static uint8_t  data[512];

void USB_Acm_BulkOut_Hdr(void)
{
	uint32_t nCount;
	//uint8_t data[512];

	nCount = hal_usb_get_rx_packet_length(g_UsbACM.rxpipe->byEP);
	if(nCount != 0)
	{
		hal_usb_read_endpoint_fifo(g_UsbACM.rxpipe->byEP, nCount, data);
		hal_usb_set_endpoint_rx_ready(g_UsbACM.rxpipe->byEP);
	}
	else
	{
		/* Clear RxPktRdy, drop this packet */
		hal_usb_set_endpoint_rx_ready(g_UsbACM.rxpipe->byEP);
	}

	//DMA echo test
	hal_usb_stop_dma_channel(g_UsbACM.txpipe->byEP, HAL_USB_EP_DIRECTION_TX);
	hal_usb_start_dma_channel(g_UsbACM.txpipe->byEP, HAL_USB_EP_DIRECTION_TX,HAL_USB_EP_TRANSFER_BULK, data, nCount, NULL, KAL_TRUE, HAL_USB_DMA1_TYPE);
}


/* EP Bulk In reset handler */
void USB_Acm_BulkIn_Reset(void)
{
	g_UsbACM.txpipe = &g_UsbACM.data_ep_in_info->ep_status.epin_status;
}


/* EP Bulk Out reset handler */
void USB_Acm_BulkOut_Reset(void)
{
	g_UsbACM.rxpipe = &g_UsbACM.data_ep_out_info->ep_status.epout_status;
}

/* When PC return ACK, handle ACM intr pipe state machine */
void USB_Acm_IntrIn_Hdlr(void)
{
	uint32_t len;
	uint32_t max_intr_pkt_size;

	if(gUsbDevice.nDevState != DEVSTATE_CONFIG)
	{
		/* USB reset */
		//USB_Dbg_Trace(USB_ACM_INTERRUPT_RESET, 0, 0);
		g_UsbACM.intr_state = ACM_INTR_IDLE;
		return;
	}

	max_intr_pkt_size = USB_Intr_Max_Packet_Size();

	switch(g_UsbACM.intr_state)
	{
	case ACM_INTR_SEND_ONE_PKT:
		g_UsbACM.intr_state = ACM_INTR_SEND_LAST_PKT;

		len = ((uint32_t)sizeof(UsbAcm_Intr_Pkt)) - max_intr_pkt_size;

		/* send last short packet */
		hal_usb_write_endpoint_fifo(g_UsbACM.intrpipe->byEP, len, (uint8_t *)(uint32_t)&g_UsbACM.intr_pkt + max_intr_pkt_size);
		hal_usb_set_endpoint_tx_ready(g_UsbACM.intrpipe->byEP);
		break;

	case ACM_INTR_SEND_LAST_PKT:
		g_UsbACM.intr_state = ACM_INTR_IDLE;

		/* Send another intr pkt */
		if(g_UsbACM.is_intr_pending_pkt == true)
		{
			g_UsbACM.is_intr_pending_pkt = false;
		}
		break;

	case ACM_INTR_IDLE:
	default:
		break;
	}
}


void USB_Acm_IntrIn_Reset(void)
{

	g_UsbACM.intr_state = ACM_INTR_IDLE;
}
