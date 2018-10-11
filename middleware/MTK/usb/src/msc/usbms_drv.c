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


/*****************************************************************************
 *
 * Filename:
 * ---------
 *    usbms_drv.c
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *   This file implements usb MASS STORAGE class driver
 *
 * Author:
 * -------
 *   Jensen Hu
 *   CJung Chen
 *
 *****************************************************************************/

#ifdef  __USB_MASS_STORAGE_ENABLE__

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "usb.h"
#include "usb_resource.h"

#include "usbms_drv.h"
#include "usbms_adap.h"
#include "usb_custom.h"
#include "syslog.h"
#include "kal_data_types.h"
#include "bmd.h"

UsbMs_Struct g_UsbMS;
usb_ms_rec_done_conf_struct g_rec_done_conf;

/* static functions */
static void USB_Ms_Rx_DMA_Callback(void);
static void USB_Ms_Tx_DMA_Callback(void);
static void USB_Ms_Ep0_Command(Usb_Ep0_Status *pep0state, Usb_Command *pcmd);
static void USB_Ms_BulkOut_Hdlr(void);
static void USB_Ms_BulkIn_Reset(void);
static void USB_Ms_BulkOut_Reset(void);
static void USB_Ms_Cmd_Reset(void);

const uint8_t ms_if_dscr[] = {
    USB_IFDSC_LENGTH,	//bLength;
    USB_INTERFACE,	//bDescriptorType;
    0x00,	//bInterfaceNumber;
    0x00,	//bAlternateSetting;
    USB_MS_EP_NUMBER,	//bNumEndpoints;
    USB_MS_INTERFACE_CLASS_CODE,	//bInterfaceClass;
    USB_MS_INTERFACE_SUBCLASS_CODE,	//bInterfaceSubClass;
    USB_MS_INTERFACE_PROTOCOL_CODE,	//bInterfaceProtocol;
    0x00	//iInterface;
};

extern const uint8_t usb_epbulk_dscr[];

/************************************************************
	TX/RX path build functions(use DMA)
*************************************************************/

/* Prepare to receive RX packet to addr and length len */
void USB_Ms_BuildRx(void *addr, uint32_t len)
{
    /* For CBW is 31 */
    if (len == 31) {
        LOG_E(hal, "ASSERT\n");
    }

    /* Stop and ack DMA if previous DMA is not yet ready */
    hal_usb_stop_dma_channel(g_UsbMS.rxpipe->byEP, HAL_USB_EP_DIRECTION_RX);

    /* Initialize rxpipe data */
    g_UsbMS.rxpipe->pData = (uint8_t *)addr;
    g_UsbMS.rxpipe->nBytesRecv = 0;
    g_UsbMS.rxpipe->nBuffLen = (int32_t)len;

    NVIC_DisableIRQ(USB20_IRQn);//IRQMask(IRQ_USB_CODE);

    /* USB reset and MS ep0 reset cmd will both stop DMA */
    /* If they happen before IRQmask, we will not build DMA in the following if conditions */
    /* If they happen after IRQmask, they will stop DMA */
    /* If before they stop DMA, the DMA already done, this DMA callback message will be ignored by USB task if checking */

    //After USB Reset or USB_EP0_Command_Reset, we must build DMA RX for the first CBW (reset = True)(nState = IDLE)
    //make sure : handle USB Reset message first. (ex. going to build RX + USB_RESET+ Build RX+ USB_RESET_Message(build RX))
    if ((g_UsbMS.usb_reset == false) && (g_UsbMS.ms_is_reset == false)) {
        /* DMA running state is NOT cleared by USB_Ms_Rx_DMA_Callback */
        hal_usb_start_dma_channel(g_UsbMS.rxpipe->byEP, HAL_USB_EP_DIRECTION_RX, HAL_USB_EP_TRANSFER_BULK, (void *)addr, len, USB_Ms_Rx_DMA_Callback, false, HAL_USB_DMA1_TYPE);
    }

    NVIC_EnableIRQ(USB20_IRQn);//USB_IRQ_Unmask();

}


/* Prepare to transmit Tx packet from addr and length len */
void USB_Ms_BuildTx(void *addr, uint32_t len)
{
    /* Stop and ack DMA if previous DMA is not yet ready */
    hal_usb_stop_dma_channel(g_UsbMS.txpipe->byEP, HAL_USB_EP_DIRECTION_TX);

    /* Initialize TX pipe */
    g_UsbMS.txpipe->pData = (uint8_t *)addr;
    g_UsbMS.txpipe->nBytesLeft = (int32_t)len;

    NVIC_DisableIRQ(USB20_IRQn);//IRQMask(IRQ_USB_CODE);

    /* USB reset and MS ep0 reset cmd will both stop DMA */
    /* If they happen before IRQmask, we will not build DMA in the following if conditions */
    /* If they happen after IRQmask, they will stop DMA */
    /* If before they stop DMA, the DMA already done, this DMA callback message will be ignored by USB task's if checking */
    if ((g_UsbMS.usb_reset == false) && (g_UsbMS.ms_is_reset == false)) {
        /* For CSW is 13 */
        if (len != 13) {
            /* DMA running state is NOT cleared by USB_Ms_Tx_DMA_Callback */
            hal_usb_start_dma_channel(g_UsbMS.txpipe->byEP, HAL_USB_EP_DIRECTION_TX, HAL_USB_EP_TRANSFER_BULK, (void *)addr, len, USB_Ms_Tx_DMA_Callback, false, HAL_USB_DMA1_TYPE);
        } else {
            /* DMA running state is NOT cleared by USB_Ms_Tx_DMA_Callback */
            hal_usb_start_dma_channel(g_UsbMS.txpipe->byEP, HAL_USB_EP_DIRECTION_TX, HAL_USB_EP_TRANSFER_BULK, (void *)addr, len, NULL, false, HAL_USB_DMA1_TYPE);
        }
    }

    NVIC_EnableIRQ(USB20_IRQn);//USB_IRQ_Unmask();
}


/************************************************************
	interface initialization functions
*************************************************************/

/* interface create function, prepare descriptor */
void USB_Ms_If_Create(void *ifname)
{
    uint8_t ep_in_id;
    uint8_t ep_out_id;
    uint8_t if_id;

    /* get resource number and register to gUsbDevice */
    g_UsbMS.ms_if_info = USB_Get_Interface_Number(&if_id);
    g_UsbMS.ms_ep_in_info = USB_Get_Bulk_Tx_Ep(&ep_in_id);
    g_UsbMS.ms_ep_out_info = USB_Get_Bulk_Rx_Ep(&ep_out_id);

    /* record interface name and interface descriptor length */
    g_UsbMS.ms_if_info->interface_name_ptr = (char *)ifname;
    g_UsbMS.ms_if_info->ifdscr_size = USB_IFDSC_LENGTH;
    g_UsbMS.ms_ep_in_info->epdscr_size = USB_EPDSC_LENGTH;
    g_UsbMS.ms_ep_out_info->epdscr_size = USB_EPDSC_LENGTH;

    /* related endpoint info and class specific command handler*/
    g_UsbMS.ms_if_info->ep_info[0] = (Usb_Ep_Info *)g_UsbMS.ms_ep_in_info;
    g_UsbMS.ms_if_info->ep_info[1] = (Usb_Ep_Info *)g_UsbMS.ms_ep_out_info;
    g_UsbMS.ms_if_info->if_class_specific_hdlr = USB_Ms_Ep0_Command;

    memcpy(&(g_UsbMS.ms_if_info->ifdscr.stdif), ms_if_dscr, USB_IFDSC_LENGTH);
    memcpy(&(g_UsbMS.ms_ep_in_info->epdesc.stdep), usb_epbulk_dscr, USB_EPDSC_LENGTH);
    memcpy(&(g_UsbMS.ms_ep_out_info->epdesc.stdep), usb_epbulk_dscr, USB_EPDSC_LENGTH);

    g_UsbMS.ms_if_info->ifdscr.stdif.bInterfaceNumber = if_id;
    g_UsbMS.ms_if_info->ifdscr.stdif.iInterface = USB_Get_String_Number((void *)g_UsbMS.ms_param->interface_string);

    /* endpoint handler */
    /* TX Endpoint handler */
    hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_EP_TX, ep_in_id, NULL);
    g_UsbMS.ms_ep_in_info->ep_reset = USB_Ms_BulkIn_Reset;
    hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_EP_RX, ep_out_id, USB_Ms_BulkOut_Hdlr);


    g_UsbMS.ms_ep_out_info->ep_reset = USB_Ms_BulkOut_Reset;

    g_UsbMS.ms_ep_in_info->epdesc.stdep.bEndpointAddress = (USB_EP_DIR_IN | ep_in_id);	/*InPipe*/
    g_UsbMS.ms_ep_in_info->ep_status.epin_status.byEP = ep_in_id;
    g_UsbMS.ms_ep_in_info->ep_status.epin_status.nBytesLeft = USB_EP_NODATA;
    g_UsbMS.ms_ep_out_info->epdesc.stdep.bEndpointAddress = (USB_EP_DIR_OUT | ep_out_id);	/*OutPipe*/
    g_UsbMS.ms_ep_out_info->ep_status.epout_status.byEP = ep_out_id;

    hal_usb_get_dma_channel(ep_in_id, ep_out_id, HAL_USB_EP_DIRECTION_TX, true);
}


/* interface reset function */
void USB_Ms_If_Reset(void)
{
    uint8_t index;

    /* This must not exist because use this trivky to avoid reset abnormal operation when writing to flash*/
    g_UsbMS.txpipe = &g_UsbMS.ms_ep_in_info->ep_status.epin_status;
    g_UsbMS.rxpipe = &g_UsbMS.ms_ep_out_info->ep_status.epout_status;

    g_UsbMS.usb_reset = true;
    g_UsbMS.ms_read_write_reset = true;

    for (index = 0; index < MAX_DISK_DRVER; index++) {
        g_UsbMS.power_off_state[index] = false;
    }

    /* Stop DMA */
    hal_usb_stop_dma_channel(g_UsbMS.rxpipe->byEP, HAL_USB_EP_DIRECTION_RX);

}

/* interface reset function */
void USB_Ms_If_Resume(void)
{
    uint8_t index;

    for (index = 0; index < MAX_DISK_DRVER; index++) {
        g_UsbMS.power_off_state[index] = false;
    }
}



/* interface enable function, enable EP*/
void USB_Ms_If_Enable(void)
{
    /* Stop DMA */
    /* Compliance test tool will set configuration , but no reset */
    hal_usb_stop_dma_channel(g_UsbMS.rxpipe->byEP, HAL_USB_EP_DIRECTION_RX);

    /* Configure DMA setting for EP */
    hal_usb_enable_tx_endpoint(g_UsbMS.txpipe->byEP, HAL_USB_EP_TRANSFER_BULK, HAL_USB_EP_USE_ONLY_DMA, true);
    hal_usb_enable_rx_endpoint(g_UsbMS.rxpipe->byEP, HAL_USB_EP_TRANSFER_BULK, HAL_USB_EP_USE_NO_DMA, true);

    /* We do not dynamically switch TX's usage, since if we config TX DMA, and in DMA callback, we switch to FIFO,
    but last packet is sent out after we switch to FIFO, IP will give a ep intr for this DMA "sending" packet, so we will see two ep intrs */

    /* Send reset message to usb task, task handle the ms state machine */
    if (g_UsbMS.is_send_usb_reset_ilm == true) {
        g_UsbMS.is_send_usb_reset_ilm = false;
		USB_Send_Message(USB_MSC_RESET_IND_MSG, NULL);
    }
}


/* interface speed reset function, change EP's speed-specific descriptor */
void USB_Ms_If_Speed_Reset(bool b_other_speed)
{

#if 1//def __USB_HS_ENABLE__ 

    uint32_t  max_packet;
    max_packet = USB_Speed_Reset_Packet_Size(b_other_speed);


    g_UsbMS.ms_ep_in_info->epdesc.stdep.wMaxPacketSize[0] = max_packet & 0xff;
    g_UsbMS.ms_ep_in_info->epdesc.stdep.wMaxPacketSize[1] = (max_packet >> 8) & 0xff;

    g_UsbMS.ms_ep_out_info->epdesc.stdep.wMaxPacketSize[0] = max_packet & 0xff;
    g_UsbMS.ms_ep_out_info->epdesc.stdep.wMaxPacketSize[1] = (max_packet >> 8) & 0xff;

#endif
}


/************************************************************
	global variable g_UsbMS initialize and release functions
*************************************************************/

/* initialize global variable g_UsbMS */
void USB_Init_Ms_Status(void)
{
    uint8_t index;
    LOG_E(hal, "USB_Init_Ms_Status\n");

    g_UsbMS.nState = USBMS_IDLE;
    g_UsbMS.txpipe = NULL;
    g_UsbMS.rxpipe = NULL;
    g_UsbMS.max_LUN = USB_Ms_Get_Max_LUN();
    g_UsbMS.current_LUN = 0;
    g_UsbMS.is_send_usb_reset_ilm = true;
    g_UsbMS.cbw_is_vaild = true;
    g_UsbMS.ms_is_reset = false;
    g_UsbMS.usb_reset = false;
    g_UsbMS.CSWDataResidue = 0;
    g_UsbMS.CSWStatusError = 0x00;
    g_UsbMS.sensePtr = NULL;

    if (g_UsbMS.disk_buffer == NULL) {
        LOG_E(hal, "USB_Init - disk_buffer\n");
        g_UsbMS.disk_buffer = (void *)USB_Get_Memory (USBMS_MAX_BUFFERSIZE); //(void*)USB_TEST_Buffer;
    }

    for (index = 0; index < MAX_DISK_DRVER; index++) {
        g_UsbMS.power_off_state[index] = false;
    }
}


/* release global variable g_UsbMS */
void USB_Release_Ms_Status(void)
{
    hal_usb_release_dma_channel(g_UsbMS.txpipe->byEP, g_UsbMS.rxpipe->byEP, HAL_USB_EP_DIRECTION_TX, true);
    USB_Init_Ms_Status();
	USB_Free_Memory();
	USB_Ms_DeRegister_DiskDriver(g_UsbMS.current_LUN);
    g_UsbMS.disk_buffer = NULL;
}


/************************************************************
	Tx/Rx path DMA callback function
*************************************************************/

/* RX DMA callback function*/
static void USB_Ms_Rx_DMA_Callback(void)
{
    usb_ms_rec_done_conf_struct *rec_done_conf;


    /* MS reset cmd must re start from RX CBW */
    if (USB_Get_Device_State() == DEVSTATE_CONFIG) {
        g_UsbMS.rxpipe->nBytesRecv = g_UsbMS.rxpipe->nBuffLen;
        /* let ms state machine handle the received data */
        g_rec_done_conf.pData = g_UsbMS.rxpipe->pData;
        g_rec_done_conf.nBuffLen = g_UsbMS.rxpipe->nBuffLen;
        g_rec_done_conf.nBytesRecv = g_UsbMS.rxpipe->nBytesRecv;
        rec_done_conf =  &g_rec_done_conf;

        /* After USB_reset HISR and before stop DMA, a DMA callbcak happen,
        this callback message may come after reset message, and make MS state machine error */
        if ((USB_Get_Device_State() == DEVSTATE_CONFIG) && (g_UsbMS.ms_is_reset == false)) {
			USB_Send_Message(USB_MSC_RX_MSG, rec_done_conf);
        }
    }
}


/* TX DMA callback function */
static void USB_Ms_Tx_DMA_Callback(void)
{
    if (USB_Get_Device_State() == DEVSTATE_CONFIG) {
        g_UsbMS.txpipe->nBytesLeft = USB_EP_NODATA;

        /*let ms state machine handle the transmitted data*/
        /* After USB_reset HISR and before stop DMA, a DMA callbcak happen,
        this callback message may come after reset message, and make MS state machine error */
                  
        if ((USB_Get_Device_State() == DEVSTATE_CONFIG) && (g_UsbMS.ms_is_reset == false)) {
			USB_Send_Message(USB_MSC_TX_MSG, NULL);
        }
    }
}


/************************************************************
	EP0 command parse functions
*************************************************************/
hal_usb_endpoint_state_t hal_usb_check_rx_endpoint_usage(uint32_t ep_num);
hal_usb_status_t hal_usb_configure_rx_endpoint_type(uint32_t ep_num, hal_usb_endpoint_transfer_type_t ep_type, bool b_is_use_dma);

/* Reset command handler */
static void USB_Ms_Cmd_Reset(void)
{
    hal_usb_stop_dma_channel(g_UsbMS.rxpipe->byEP, HAL_USB_EP_DIRECTION_RX);

    hal_usb_clear_tx_endpoint_fifo(g_UsbMS.txpipe->byEP, HAL_USB_EP_TRANSFER_BULK, true);

    hal_usb_clear_rx_endpoint_fifo(g_UsbMS.rxpipe->byEP, HAL_USB_EP_TRANSFER_BULK, true);

    /* Switch endpoint state to use FIFOread */
	if (hal_usb_check_rx_endpoint_usage(g_UsbMS.rxpipe->byEP) == HAL_USB_EP_STATE_DMA) {
		hal_usb_configure_rx_endpoint_type(g_UsbMS.rxpipe->byEP, HAL_USB_EP_TRANSFER_BULK, false);
	}

    /* Send reset message to usb task, task handle the state machine */
    if (g_UsbMS.is_send_usb_reset_ilm == true) {
        /* avoid sending too many messages to task when task is busy writing*/
        g_UsbMS.is_send_usb_reset_ilm = false;
		USB_Send_Message(USB_MSC_RESET_IND_MSG, NULL);
    }
}


/* parse MS class specific command */
static void USB_Ms_Ep0_Command(Usb_Ep0_Status *pep0state, Usb_Command *pcmd)
{
    bool  bError = false;

    //LOG_E(hal, "USB_Ms_Ep0_Command, req=0x%x\n", pcmd->bRequest);

    switch (pcmd->bRequest) {
        case USB_MS_RESET:
            if ( (pcmd->bmRequestType != USB_CMD_CLASSIFOUT) || (pcmd->wValue != 0) ) {
                bError = true;
            } else {
                /* In USB compliance tool, PC will send MS ep0 reset cmd, and then clear ep feature, and send CBW */
                g_UsbMS.cbw_is_vaild = true;
                g_UsbMS.ms_is_reset = true;
                /* Build DMA RX here */
                USB_Ms_Cmd_Reset();
                /* Must clear stall status here */
                hal_usb_set_endpoint_stall(g_UsbMS.txpipe->byEP, HAL_USB_EP_DIRECTION_TX, false);
                hal_usb_set_endpoint_stall(g_UsbMS.rxpipe->byEP, HAL_USB_EP_DIRECTION_RX, false);
            }
            break;

        case USB_MS_GETMAXLUN:
            if ( (pcmd->bmRequestType != USB_CMD_CLASSIFIN) || (pcmd->wValue != 0) ) {
                bError = true;
            } else {
                g_UsbMS.max_LUN = USB_Ms_Get_Max_LUN();

                LOG_E(hal, "USB_Ms_Get_Max_LUN, lun=%d\n", g_UsbMS.max_LUN);
                USB_Generate_EP0Data(pep0state, pcmd, (void *)&g_UsbMS.max_LUN, 1);
            }
            break;

        default:
            bError = true;
            break;
    }

    USB_EP0_Command_Hdlr(bError);
}


/* EP Bulk Out interrupt handler, called by EP interrupt */
static void USB_Ms_BulkOut_Hdlr(void)
{
    uint32_t nCount;
    usb_ms_rec_done_conf_struct *rec_done_conf;

    /* Check received packet length */
    nCount = hal_usb_get_rx_packet_length(g_UsbMS.rxpipe->byEP);

    if (nCount == 31) {
        hal_usb_read_endpoint_fifo(g_UsbMS.rxpipe->byEP, 31,  g_UsbMS.CBWdata);
        g_UsbMS.rxpipe->nBytesRecv = 31;
        g_UsbMS.rxpipe->pData = (uint8_t *)g_UsbMS.CBWdata;
        g_UsbMS.rxpipe->nBuffLen = 31;

        /* let ms state machine handle the received data */
        g_rec_done_conf.pData = g_UsbMS.rxpipe->pData;
        g_rec_done_conf.nBuffLen = g_UsbMS.rxpipe->nBuffLen;
        g_rec_done_conf.nBytesRecv = g_UsbMS.rxpipe->nBytesRecv;
        rec_done_conf =  &g_rec_done_conf;

        /* After USB_reset HISR and before stop DMA, a DMA callbcak happen,
        this callback message may come after reset message, and make MS state machine error */
        if ((USB_Get_Device_State() == DEVSTATE_CONFIG) && (g_UsbMS.ms_is_reset == false)) {
			USB_Send_Message(USB_MSC_RX_MSG, rec_done_conf);
        }

		hal_usb_configure_rx_endpoint_type(g_UsbMS.rxpipe->byEP, HAL_USB_EP_TRANSFER_BULK, true);

        hal_usb_set_endpoint_rx_ready(g_UsbMS.rxpipe->byEP);

    } else {
        /* Error handling */
        LOG_E(hal, "USBMS_RX_WRONG_CBW_LENGTH=%d\n", nCount);

        g_UsbMS.cbw_is_vaild = false;
        hal_usb_set_endpoint_stall(g_UsbMS.txpipe->byEP, HAL_USB_EP_DIRECTION_TX, true);
        hal_usb_set_endpoint_stall(g_UsbMS.rxpipe->byEP, HAL_USB_EP_DIRECTION_RX, true);
    }
}


/* Bulk In endpoint reset function */
static void USB_Ms_BulkIn_Reset(void)
{
    return;
}


/* Bulk Out endpoint reset function */
static void USB_Ms_BulkOut_Reset(void)
{
    return;
}

#endif //__USB_MASS_STORAGE_ENABLE__


