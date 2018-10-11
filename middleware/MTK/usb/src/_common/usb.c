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

#include "hal_usb.h"
#include "usb_comm.h"
#include "usb.h"
#include "usb_resource.h"
#include "usbacm_drv.h"
#include "usb_custom.h"
#include "usb_custom_def.h"
#include "syslog.h"
#include "memory_attribute.h"

#include "usbms_state.h"
#include "usbms_drv.h"


/* Device structure, handle usb level data */
Usb_Device gUsbDevice;

/* static fuctions */
static void USB_Init_Device_Status(void);
static void USB_Reset(void);
static void USB_Suspend(void);
static void USB_Resume(void);
static void USB_Initialize(bool double_fifo);
static bool USB_Cmd_SetAddress(Usb_Ep0_Status *pep0state, Usb_Command *pcmd);
static bool USB_Cmd_GetDescriptor(Usb_Ep0_Status *pep0state, Usb_Command *pcmd);
static bool USB_Cmd_SetConfiguration(Usb_Ep0_Status *pep0state, Usb_Command *pcmd);
static bool USB_Cmd_GetConfiguration(Usb_Ep0_Status *pep0state, Usb_Command *pcmd);
static bool USB_Cmd_SetFeature(Usb_Command *pcmd, bool bset);
static bool USB_Cmd_GetStatus(Usb_Ep0_Status *pep0state, Usb_Command *pcmd);
static void USB_Stdcmd(Usb_Ep0_Status *pep0state, Usb_Command *pcmd);
static void USB_Endpoint0_Idle (void);
static void USB_Endpoint0_Rx(void);
static void USB_Endpoint0_Tx(void);
static void USB_Endpoint0_Hdlr(void);

const unsigned char usb_epbulk_dscr[] = {
    USB_EPDSC_LENGTH,	//bLength;
    USB_ENDPOINT,	//bDescriptorType;
    0x00,	//bEndpointAddress;
    USB_EP_BULK,	//bmAttributes;
    0x40,	//wMaxPacketSize[2];
    0x00,	//wMaxPacketSize[2];
    0x00	//bInterval;
};


/************************************************************
	Adaptor
*************************************************************/
unsigned int USB_Bulk_Max_Packet_Size(void)
{
    unsigned int max_bulk_pkt_size;

    if (hal_usb_is_high_speed() == true) {
        max_bulk_pkt_size = HAL_USB_MAX_PACKET_SIZE_ENDPOINT_BULK_HIGH_SPEED;
    } else {
        max_bulk_pkt_size = HAL_USB_MAX_PACKET_SIZE_ENDPOINT_BULK_FULL_SPEED;
    }

    return max_bulk_pkt_size;
}

unsigned int USB_Intr_Max_Packet_Size(void)
{
    unsigned int max_intr_pkt_size;

    if (hal_usb_is_high_speed() == true) {
        max_intr_pkt_size = HAL_USB_MAX_PACKET_SIZE_ENDPOINT_INTERRUPT_HIGH_SPEED;
    } else {
        max_intr_pkt_size = HAL_USB_MAX_PACKET_SIZE_ENDPOINT_INTERRUPT_FULL_SPEED;
    }

    return max_intr_pkt_size;
}

unsigned int USB_Speed_Reset_Packet_Size(bool b_other_speed)
{
    unsigned int  max_packet;

    if (hal_usb_is_high_speed() == true) {

        if (b_other_speed == false) {
            max_packet = HAL_USB_MAX_PACKET_SIZE_ENDPOINT_BULK_HIGH_SPEED;
        } else {
            max_packet = HAL_USB_MAX_PACKET_SIZE_ENDPOINT_BULK_FULL_SPEED;
        }
    } else {
        if (b_other_speed == false) {
            max_packet = HAL_USB_MAX_PACKET_SIZE_ENDPOINT_BULK_FULL_SPEED;
        } else {
            max_packet = HAL_USB_MAX_PACKET_SIZE_ENDPOINT_BULK_HIGH_SPEED;
        }
    }

    return max_packet;
}

/************************************************************
	gUsbDevice initialize and release functions
*************************************************************/

/* initialize the  global variable gUsbDevice */
static void USB_Init_Device_Status(void)
{
    uint32_t 	index = 0;

    uint32_t temp_addr = (uint32_t)&gUsbDevice.resource_ep_bulk_tx_number;
    uint32_t temp_addr2 = (uint32_t)&gUsbDevice.conf;


    memset(&gUsbDevice.resource_ep_bulk_tx_number, 0, temp_addr2 - temp_addr);

    gUsbDevice.usb_test_type = HAL_USB_TEST_MODE_TYPE_NONE;
    gUsbDevice.device_type = USB_UNKOWN;
    gUsbDevice.remoteWk = false;
    gUsbDevice.self_powered = false;
    gUsbDevice.config_num = 0;

    for (index = 0; index < USB_MAX_INTERFACE; index++) {
        gUsbDevice.interface_num[index] = 0;
    }

    gUsbDevice.ep0_rx_handler = NULL;
    gUsbDevice.ep0_class_cmd_handler.b_enable = false;

}


/* release the already get memory, and reset some parameter,
    note that hisr should not be reset to NULL since it may already be created*/
void USB_Release_Device_Status(void)
{
    USB_Init_Device_Status();

}

USB_DEVICE_STATE USB_Get_Device_State(void)
{
    return gUsbDevice.nDevState;
}

USB_DEVICE_TYPE USB_Get_Device_Type(void)
{
    return gUsbDevice.device_type;
}


/************************************************************
	EP0 functions
*************************************************************/


void USB_Register_EP0_Class_CmdHdlr(uint8_t cmd, usb_ep0_cmd_ptr handler)
{
    gUsbDevice.ep0_class_cmd_handler.cmd = cmd;
    gUsbDevice.ep0_class_cmd_handler.ep0_cmd_hdlr = handler;
    gUsbDevice.ep0_class_cmd_handler.b_enable = true;
}


/* Prepare TX data infomration for pep0state, data is actually sent out in TX state handler */
void USB_Generate_EP0Data(Usb_Ep0_Status *pep0state, Usb_Command *pcmd, void *data, int32_t len)
{
    pep0state->pData = (void *)data;

    pep0state->nBytesLeft = len;

    /* Only transmit at most command request */
    if (pcmd->wLength < pep0state->nBytesLeft) {
        pep0state->nBytesLeft = pcmd->wLength;
    }

    /* No need ZLP when wLength == transfer size */
    if (pcmd->wLength == pep0state->nBytesLeft) {
        pep0state->no_ZLP = true;
    } else {
        pep0state->no_ZLP = false;
    }

    /* EP0 go to TX state */
    gUsbDevice.ep0_state = USB_EP0_TX;
}

/************************************************************
	Memory control APIs
*************************************************************/

#define USB_COM_FUNCTION_SIZE 	 ((USB_ACM_RX_BUFFER_NUM*USB_ACM_RX_BUFFER_SIZE)+USB_RX_RING_BUFFER_1_SIZE+USB_TX_RING_BUFFER_1_SIZE)
#define USB_2COM_FUNCTION_SIZE 	 (USB_COM_FUNCTION_SIZE+USB_RX_RING_BUFFER_2_SIZE+USB_TX_RING_BUFFER_2_SIZE)
#define USB_MS_FUNCTION_SIZE 	 (USBMS_MAX_BUFFERSIZE)

#if USB_MS_FUNCTION_SIZE > USB_2COM_FUNCTION_SIZE
#define USB_MAX_MEMORY_SIZE		(USB_MS_FUNCTION_SIZE + USB_DESCRIPTOR_SIZE)
#else
#define USB_MAX_MEMORY_SIZE		(USB_2COM_FUNCTION_SIZE+ USB_DESCRIPTOR_SIZE)
#endif


ATTR_RWDATA_IN_NONCACHED_RAM unsigned char USB_BUFFER[USB_MAX_MEMORY_SIZE]; // USB total buffer

unsigned int USB_Get_Memory(unsigned int len)
{
    unsigned int address;

    if (len & 0xF) { // for better performance, use 16 B align address
        len = (len + 16) & ~(0xF);
    }

    if ((gUsbDevice.reserved_memory_size == 0) && (gUsbDevice.memory_addr == 0)) {
        gUsbDevice.memory_addr = (unsigned int)USB_BUFFER;
        gUsbDevice.reserved_memory_size = USB_MAX_MEMORY_SIZE;
    }

    if ((gUsbDevice.reserved_memory_size < len) || (gUsbDevice.memory_addr == 0)) {
        LOG_I(hal, "ASSERT, USB_Get_Memory\n");
    }

    gUsbDevice.reserved_memory_size = gUsbDevice.reserved_memory_size - len;
    address = gUsbDevice.memory_addr;
    gUsbDevice.memory_addr = gUsbDevice.memory_addr + len;

    return address;
}

void USB_Free_Memory(void)
{
    gUsbDevice.memory_addr = (unsigned int)USB_BUFFER;
    gUsbDevice.reserved_memory_size = USB_MAX_MEMORY_SIZE;
}


/************************************************************
	system ctrl functions
*************************************************************/

/* Init function, called when user select usb type,
    entry function for task , B_eanble is D+ detection enable */
void USB_Init(USB_DEVICE_TYPE type)
{

    hal_usb_configure_driver();
    USB_Init_Device_Status();	/* Initialize global variable gUsbDevice */

    switch (type) {
        case USB_CDC_ACM:
            USB_Init_Acm_Status();
            break;

#ifdef  __USB_MASS_STORAGE_ENABLE__
        case USB_MASS_STORAGE:
            USB_Init_Ms_Status();
            break;
#endif

        default:
            LOG_E(hal, "ASSERT\n");
            break;
    }
    gUsbDevice.device_type = type;

    /* Register reset and ep0 interrupt handler to driver info */
    hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_RESET, 0, USB_Reset);
    hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_EP0, 0, USB_Endpoint0_Hdlr);
    hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_SUSPEND, 0, USB_Suspend);
    hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_RESUME, 0, USB_Resume);



#if 0//def __USB_RAMDISK__
    if (type == USB_MASS_STORAGE) {
        FAT_Init();
    }
#endif   /*__USB_RAMDISK__*/


    /* Create USB descriptors */
    USB_Software_Create();

    /* Reset and initialize system initial value and registers*/
    hal_usb_reset_hardware();
    USB_Initialize(false);

    /* Set D+ to high finally */
    hal_usb_pull_up_dp_line();
}

/* The enable parameter decides whether to turn off D+ at this time */
void USB_Release_Type(void)
{
    gUsbDevice.nDevState = DEVSTATE_DEFAULT;

    /* Stop and release resource */
    switch (gUsbDevice.device_type) {
        case USB_CDC_ACM:
            USB_Release_Acm_Status();
            break;
#if defined(__USB_MASS_STORAGE_ENABLE__)
        case USB_MASS_STORAGE:
            USB_Release_Ms_Status();
            break;
#endif
    }
    USB_DeRegister_CreateFunc();

    /* power down USB */
    /* Disable D+ pull high */
    hal_usb_deinit();
}

/* Reset device, called when receive reset interrupt */
extern void hal_usb_reset_drv_info(void);

static void USB_Reset(void)
{
    uint32_t ep_num;

    hal_usb_reset_drv_info();

    // Initialize driver info and system interrupt
    USB_Initialize(false);

    gUsbDevice.nDevState = DEVSTATE_DEFAULT;

    for (ep_num = 0; ep_num < gUsbDevice.resource_ep_bulk_tx_number; ep_num++) {
        if (gUsbDevice.ep_bulk_tx_info[ep_num].ep_reset) {
            gUsbDevice.ep_bulk_tx_info[ep_num].ep_reset();
        }
    }

    for (ep_num = 0; ep_num < gUsbDevice.resource_ep_bulk_rx_number; ep_num++) {
        if (gUsbDevice.ep_bulk_rx_info[ep_num].ep_reset) {
            gUsbDevice.ep_bulk_rx_info[ep_num].ep_reset();
        }
    }

    for (ep_num = 0; ep_num < gUsbDevice.resource_ep_intr_tx_number; ep_num++) {
        if (gUsbDevice.ep_intr_tx_info[ep_num].ep_reset) {
            gUsbDevice.ep_intr_tx_info[ep_num].ep_reset();
        }
    }
}


static void USB_Suspend(void)
{
}


static void USB_Resume(void)
{

}

/* initialize system */
static void USB_Initialize(bool double_fifo)
{
    uint32_t index_ep;
    uint16_t wMaxPacketSize;
    uint32_t 	index;
    bool	temp_fifo;

    /* initial class specific speed-related endpoint, and re-prepare descriptor */
    USB_Software_Speed_Init(false);

    hal_usb_reset_fifo();

    for (index_ep = 0; index_ep < gUsbDevice.resource_ep_bulk_tx_number; index_ep++) {
        wMaxPacketSize = USB_Bulk_Max_Packet_Size();

        temp_fifo = (index_ep == 0) ? double_fifo : false;

        hal_usb_init_tx_endpoint((gUsbDevice.ep_bulk_tx_info[index_ep].epdesc.stdep.bEndpointAddress & (~USB_EP_DIR)),
                                 wMaxPacketSize, HAL_USB_EP_TRANSFER_BULK, temp_fifo);

        gUsbDevice.ep_bulk_tx_info[index_ep].ep_status.epin_status.nBytesLeft = USB_EP_NODATA;
    }

    for (index_ep = 0; index_ep < gUsbDevice.resource_ep_bulk_rx_number; index_ep++) {
        wMaxPacketSize = USB_Bulk_Max_Packet_Size();

        temp_fifo = (index_ep == 0) ? double_fifo : false;
        hal_usb_init_rx_endpoint((gUsbDevice.ep_bulk_rx_info[index_ep].epdesc.stdep.bEndpointAddress & (~USB_EP_DIR)),
                                 wMaxPacketSize, HAL_USB_EP_TRANSFER_BULK, temp_fifo);

        gUsbDevice.ep_bulk_rx_info[index_ep].ep_status.epout_status.nBuffLen = 0;
        gUsbDevice.ep_bulk_rx_info[index_ep].ep_status.epout_status.nBytesRecv = 0;
    }

    for (index_ep = 0; index_ep < gUsbDevice.resource_ep_intr_tx_number; index_ep++) {
        wMaxPacketSize = USB_Intr_Max_Packet_Size();

        hal_usb_init_tx_endpoint((gUsbDevice.ep_intr_tx_info[index_ep].epdesc.stdep.bEndpointAddress & (~USB_EP_DIR)),
                                 wMaxPacketSize, HAL_USB_EP_TRANSFER_INTR, false);
        gUsbDevice.ep_intr_tx_info[index_ep].ep_status.epin_status.nBytesLeft = USB_EP_NODATA;
    }

    /* Clear current configuration pointer */
    gUsbDevice.self_powered = false;
    gUsbDevice.remoteWk = false;
    gUsbDevice.config_num = 0;        /*  set configuration command value  */

    for (index = 0; index < USB_MAX_INTERFACE; index++) {
        gUsbDevice.interface_num[index] = 0;
    }

    gUsbDevice.ep0_state = USB_EP0_IDLE;
    gUsbDevice.ep0info.byFAddr = 0xff;   /*  device (function) address, no use, at HW still set 0x00 */

    /* initial class specific interface functions*/
    USB_Software_Init();
}


/************************************************************
	EP0 functions
*************************************************************/

/* parse command Set Address */
static bool USB_Cmd_SetAddress(Usb_Ep0_Status *pep0state, Usb_Command *pcmd)
{
    bool bError = true;

    /* Store device function address until status stage of request */
    if (pcmd->bmRequestType == USB_CMD_STDDEVOUT) {
        if (gUsbDevice.nDevState <= DEVSTATE_ADDRESS) {
            pep0state->byFAddr = (uint8_t)pcmd->wValue;

            if ((gUsbDevice.nDevState == DEVSTATE_DEFAULT) && (pep0state->byFAddr <= 127)) {
                gUsbDevice.nDevState = DEVSTATE_SET_ADDRESS;
                hal_usb_set_address(pep0state->byFAddr, HAL_USB_SET_ADDR_DATA);
            } else {
                gUsbDevice.nDevState = DEVSTATE_DEFAULT;
            }

            bError = false;
        }
    }

    return bError;
}

/* parse command Get Descriptor */
static bool USB_Cmd_GetDescriptor(Usb_Ep0_Status *pep0state, Usb_Command *pcmd)
{
    bool bError = false;
    uint8_t byConfig;
    uint8_t bystr;
    Usb_Cfg_Dscr *pcfg;

    //LOG_E(hal, "USB_Cmd_GetDescriptor:pcmd=0x%x\n", pcmd->wValue);

    /* Decode the required descriptor from the command */
    if (pcmd->bmRequestType != USB_CMD_STDDEVIN) {
        bError = true;
    } else {
        switch (pcmd->wValue & USB_CMD_DESCMASK) {
            case USB_CMD_DEVICE:
                //LOG_I(hal, "---USB_CMD_DEVICE\n");
                /* First time host get device descriptor will only get 8 bytes (one packet), but no reset */
                if (gUsbDevice.device_type == USB_IMAGE) {
                    if (pcmd->wLength == 64) {
                        gUsbDevice.ep0_send_one_pkt = true;
                    }
                }

                /* Prepare to return Standard Device Descriptor */
                USB_Generate_EP0Data(pep0state, pcmd, &gUsbDevice.devdscr, sizeof(Usb_Dev_Dscr));
                break;

            case USB_CMD_CONFIG:
                //LOG_I(hal, "----USB_CMD_CONFIG\n");
                byConfig = (uint8_t)(pcmd->wValue & 0x00FF);
                USB_Software_Speed_Init(false);
                if (byConfig >= gUsbDevice.devdscr.bNumConfigurations) {
                    bError = true;
                } else {
                    /* Get pointer to request configuration descriptor */
                    pcfg = (Usb_Cfg_Dscr *)gUsbDevice.conf;

                    /* Memory copy to modify descriptor */
                    pcfg->bConfigurationValue = (byConfig + 1);
                    pcfg->bmAttributes |= (USB_CFGDSC_ATTR_SELFPOWER | USB_CFGDSC_ATTR_NATURE); /* Return self power */

                    /* Prepare to return Configuration Descriptors */
                    USB_Generate_EP0Data(pep0state, pcmd, pcfg, pcfg->wTotalLength);
                }
                break;

            case USB_CMD_STRING:
                //LOG_I(hal, "----USB_CMD_STRING\n");
                bystr = (uint8_t)(pcmd->wValue & 0x00FF);
                if (bystr >= gUsbDevice.resource_string_number) {
                    bError = true;
                } else {
                    /* Get pointer to requested string descriptor */
                    USB_Generate_EP0Data(pep0state, pcmd, (void *)gUsbDevice.resource_string[bystr],
                                         (*(uint8_t *)gUsbDevice.resource_string[bystr]));
                }
                break;
            case USB_CMD_DEVICE_QUALIFIER:
                //LOG_I(hal, "----USB_CMD_DEVICE_QUALIFIER\n");
                /* Prepare to return Standard Device_Qualifier Descriptor */
                USB_Generate_EP0Data(pep0state, pcmd, &gUsbDevice.dev_qual_dscr, sizeof(Usb_Dev_Qual_Dscr));
                break;

#ifdef __USB_HS_ENABLE__
            case USB_CMD_OTHER_SPEED:
                LOG_I(hal, "----USB_CMD_OTHER_SPEED\n");
                byConfig = (uint8_t)(pcmd->wValue & 0x00FF);
                USB_Software_Speed_Init(true);

                if (byConfig >= gUsbDevice.devdscr.bNumConfigurations) {
                    bError = true;
                } else {
                    /* Get pointer to requested configuration descriptor */
                    pcfg = (Usb_Cfg_Dscr *)gUsbDevice.conf;
                    /*  Memory copy to modify descriptor */
                    pcfg->bConfigurationValue = (byConfig + 1);
                    /* Prepare to return Configuration Descriptors */
                    USB_Generate_EP0Data(pep0state, pcmd, pcfg, pcfg->wTotalLength);
                }
                break;
#endif

            default:
                bError = true;
                break;
        }
    }

    return bError;
}


/* Parse command Set Configuration */
static bool USB_Cmd_SetConfiguration(Usb_Ep0_Status *pep0state, Usb_Command *pcmd)
{
    bool bError = false;
    uint8_t byConfig;

    byConfig = (uint8_t)(pcmd->wValue & 0x00FF);

    if (gUsbDevice.nDevState == DEVSTATE_DEFAULT) {
        bError = true;
    }
    /* Assumes configurations are numbered from 1 to NumConfigurations */
    else {
        if (byConfig > gUsbDevice.devdscr.bNumConfigurations) {
            bError = true;
        } else if (byConfig == 0) {
            gUsbDevice.nDevState = DEVSTATE_ADDRESS;
            gUsbDevice.config_num = 0;
        } else {
            USB_Software_Enable();

            gUsbDevice.nDevState = DEVSTATE_CONFIG;
            gUsbDevice.config_num = byConfig;
            gUsbDevice.self_powered = true; /* Return self power */
        }
    }
    return bError;
}


/* Parse command Get Configuration */
static bool USB_Cmd_GetConfiguration(Usb_Ep0_Status *pep0state, Usb_Command *pcmd)
{
    bool bError = false;


    if ( (gUsbDevice.nDevState == DEVSTATE_ADDRESS) && (!pcmd->wValue)) {
        /* Prepare to return zero */
        USB_Generate_EP0Data(pep0state, pcmd, &pcmd->wValue, 1);
    } else if ( gUsbDevice.nDevState == DEVSTATE_CONFIG) {
        /* Prepare to return configuration value */
        USB_Generate_EP0Data(pep0state, pcmd, &gUsbDevice.config_num, 1);
    } else {
        bError = true;
    }

    return bError;
}


/* Parse command Set Interface */
static bool USB_Cmd_SetInterface(Usb_Ep0_Status *pep0state, Usb_Command *pcmd)
{
    gUsbDevice.interface_num[pcmd->wIndex] = pcmd->wValue & 0x00FF;
    return false;
}


/* Parse command Get Interface */
static bool USB_Cmd_GetInterface(Usb_Ep0_Status *pep0state, Usb_Command *pcmd)
{
    USB_Generate_EP0Data(pep0state, pcmd, &gUsbDevice.interface_num[pcmd->wIndex], 1);
    return false;
}


/* Parse command Set/Clear Feature */
/* bset  true means command SET_FETURE, false means command CLEAR_FEATURE */
static bool USB_Cmd_SetFeature(Usb_Command *pcmd, bool bset)
{
    bool bError = false;

    //LOG_E(hal, "USB_Cmd_SetFeature\n");

    switch (pcmd->bmRequestType) {
        /* device */
        case USB_CMD_STDDEVOUT:

            if (pcmd->wValue == USB_FTR_DEVREMWAKE) {
                gUsbDevice.remoteWk = bset;
            } else if (pcmd->wValue == USB_FTR_TEST_MODE) {
                if (((pcmd->wIndex) >> 8) == USB_TEST_J) {
                    gUsbDevice.usb_test_type = HAL_USB_TEST_MODE_TYPE_J;
                } else if (((pcmd->wIndex) >> 8) == USB_TEST_K) {
                    gUsbDevice.usb_test_type = HAL_USB_TEST_MODE_TYPE_K;
                } else if (((pcmd->wIndex) >> 8) == USB_TEST_SE0_NAK) {
                    gUsbDevice.usb_test_type = HAL_USB_TEST_MODE_TYPE_SE0_NAK;
                } else if (((pcmd->wIndex) >> 8) == USB_TEST_PACKET) {
                    gUsbDevice.usb_test_type = HAL_USB_TEST_MODE_TYPE_PACKET;
                } else {
                    bError = true;
                }
            } else {
                bError = true;
            }
            break;
        /* endpoint */
        case USB_CMD_STDEPOUT:
            switch (pcmd->wValue) {
                case USB_FTR_EPHALT:
                    if (pcmd->wIndex != 0) {
                        if (((pcmd->wIndex & 0x80) && (((pcmd->wIndex & 0x0f) > HAL_USB_MAX_NUMBER_ENDPOINT_TX) || ((pcmd->wIndex & 0x0f) == 0x00)))
                                || (((pcmd->wIndex & 0x80) == 0) && (((pcmd->wIndex & 0x0f) > HAL_USB_MAX_NUMBER_ENDPOINT_RX) || ((pcmd->wIndex & 0x0f) == 0x00)))) {
                            return true;
                        }

#ifdef  __USB_MASS_STORAGE_ENABLE__
                        if ((gUsbDevice.device_type == USB_MASS_STORAGE) && (bset == false)) {
                            /* When stall by invalid cmd, PC must use MS reset cmd to clear stall */
                            /* Ignore this cmd, and still return stall */
                            if ((g_UsbMS.cbw_is_vaild == false) || (g_UsbMS.ms_is_reset == true)) {
                                return false;
                            }

                            if (((pcmd->wIndex & 0x80) && (hal_usb_get_endpoint_stall_status/*USB_Get_EP_Stall_Status*/(pcmd->wIndex & 0x0f, HAL_USB_EP_DIRECTION_TX) == true))
                                    || (((pcmd->wIndex & 0x80) == 0) && (hal_usb_get_endpoint_stall_status/*USB_Get_EP_Stall_Status*/(pcmd->wIndex & 0x0f, HAL_USB_EP_DIRECTION_RX) == true)))
                                //USB_Send_Msg_Ext_Queue(MOD_USB, MSG_ID_USB_MSDRV_CLEAR_STALL_REQ, NULL);

                                /* After USB_reset HISR and before stop DMA, a DMA callbcak happen,
                                this callback message may come after reset message, and make MS state machine error */
                                if ((USB_Get_Device_State() == DEVSTATE_CONFIG) && (g_UsbMS.ms_is_reset == false)) {
                                    USB_Send_Message(USB_MSC_CLR_STALL_MSG, NULL);
                                }
                        }
#endif

                        /* command EP direction zero indicate OUT EP */
                        if (pcmd->wIndex & 0x80) {
                            /* TX EP */
                            if ((bset == false) && (hal_usb_get_endpoint_stall_status(pcmd->wIndex & 0x0f, HAL_USB_EP_DIRECTION_TX) == false)) {
                                hal_usb_clear_tx_endpoint_data_toggle(pcmd->wIndex & 0x0f);
                                break;
                            }
                            hal_usb_set_endpoint_stall((pcmd->wIndex & 0x0f), HAL_USB_EP_DIRECTION_TX, bset);
                        } else {
                            /* RX EP */
                            hal_usb_set_endpoint_stall((pcmd->wIndex & 0x0f), HAL_USB_EP_DIRECTION_RX, bset);
                        }
                    }
                    break;
                default:
                    bError = true;
                    break;
            }
            break;
        case USB_CMD_STDIFOUT:
        default:
            bError = true;
            break;
    }

    return bError;
}


/* Parse command Get Status */
static bool USB_Cmd_GetStatus(Usb_Ep0_Status *pep0state, Usb_Command *pcmd)
{
    bool bError = false;
    static uint16_t status = 0;


    switch (pcmd->bmRequestType) {
        case USB_CMD_STDDEVIN:
            status = (uint16_t)(((uint8_t)gUsbDevice.remoteWk << 1) | ((uint8_t)gUsbDevice.self_powered));
            USB_Generate_EP0Data( pep0state, pcmd, &status, 2);
            break;
        case USB_CMD_STDIFIN:
            USB_Generate_EP0Data( pep0state, pcmd, &status, 2);
            break;
        case USB_CMD_STDEPIN:
            if (pcmd->wIndex & 0x80) {
                /* TX EP*/
                status = (uint16_t)hal_usb_get_endpoint_stall_status((pcmd->wIndex & 0x000f), HAL_USB_EP_DIRECTION_TX);
                //USB_Dbg_Trace(USB_EP0_GET_STATUS_TX, (uint32_t)status, 0);
            } else {
                /* RX EP*/
                status = (uint16_t)hal_usb_get_endpoint_stall_status((pcmd->wIndex & 0x000f), HAL_USB_EP_DIRECTION_RX);
                //USB_Dbg_Trace(USB_EP0_GET_STATUS_RX, status, 0);
            }
            USB_Generate_EP0Data( pep0state, pcmd, &status, 2);
            break;
        default:
            bError = true;
            break;
    }

    return bError;
}


void USB_EP0_Command_Hdlr(bool bError)
{
    if (gUsbDevice.ep0_state == USB_EP0_IDLE) {
        gUsbDevice.ep0_state = USB_EP0_RX_STATUS;
        hal_usb_update_endpoint_0_state(HAL_USB_EP0_DRV_STATE_READ_END, bError, true);
    } else {
        hal_usb_update_endpoint_0_state(HAL_USB_EP0_DRV_STATE_READ_END, bError, false);
    }
}



/* Parse usb standard command */
static void USB_Stdcmd(Usb_Ep0_Status *pep0state, Usb_Command *pcmd)
{
    bool   bError = false;


    gUsbDevice.ep0_send_one_pkt = false;

    ///* For webcam to stop DMA then reset data toggle */
    //if((gUsbDevice.ep0_class_cmd_handler.b_enable == true) &&
    //	(gUsbDevice.ep0_class_cmd_handler.cmd == pcmd->bRequest) )
    //{
    //	/* Besides standard cmd, still need to do extra this, like webcam halt feature*/
    //	gUsbDevice.ep0_class_cmd_handler.ep0_cmd_hdlr(pcmd);
    //}

    switch (pcmd->bRequest) {
        case USB_SET_ADDRESS:
            /*LOG_I(hal, "USB_SET_ADDRESS\n");*/
            bError = USB_Cmd_SetAddress(pep0state, pcmd);
            break;
        case USB_GET_DESCRIPTOR:
            /*LOG_I(hal, "USB_GET_DESCRIPTOR");*/
            bError = USB_Cmd_GetDescriptor(pep0state, pcmd);
            break;
        case USB_SET_CONFIGURATION:
            /*LOG_I(hal, "USB_SET_CONFIGURATION\n");*/
            bError = USB_Cmd_SetConfiguration(pep0state, pcmd);
            break;
        case USB_SET_INTERFACE:
            /*LOG_I(hal, "USB_SET_INTERFACE\n");*/
            bError = USB_Cmd_SetInterface(pep0state, pcmd);
            break;
        case USB_GET_CONFIGURATION:
            /*LOG_I(hal, "USB_GET_CONFIGURATION\n");*/
            bError = USB_Cmd_GetConfiguration(pep0state, pcmd);
            break;
        case USB_GET_INTERFACE:
            /*LOG_I(hal, "USB_GET_INTERFACE\n");*/
            bError = USB_Cmd_GetInterface(pep0state, pcmd);
            break;
        case USB_SET_FEATURE:
            bError = USB_Cmd_SetFeature(pcmd, true);
            break;
        case USB_CLEAR_FEATURE:
            bError = USB_Cmd_SetFeature(pcmd, false);
            break;
        case USB_GET_STATUS:
            bError = USB_Cmd_GetStatus(pep0state, pcmd);
            break;
        /* Stall the command if an unrecognized request is received */
        case USB_SYNCH_FRAME:   /*Only support for Isoc traffic*/
        case USB_SET_DESCRIPTOR:
        default:
            bError = true;
            break;
    }

    USB_EP0_Command_Hdlr(bError);

}

#if (defined(__USB_IMAGE_CLASS__)||defined(__MBIM_SUPPORT__))
/* Parse command Get MS Descriptor */
static void USB_Cmd_Get_MS_Descriptor(Usb_Ep0_Status *pep0state, Usb_Command *pcmd)
{
    bool bError = false;
    uint32_t length;

    /* Linux will sent pcmd->wIndex == USB_FTR_EXT_PROPERTIES, but MTK didn't support it now. */
#if defined(__MBIM_SUPPORT__)
    if ((pcmd->wValue == 0x00) && (pcmd->wIndex == USB_FTR_EXT_COMPAT_ID)
            && (gUsbDevice.nDevState != DEVSTATE_CONFIG) && (gUsbDevice.device_type == USB_MORPHING_MBIM)) {
        gUsbDevice.morph_active = true;
#else
    if ((pcmd->wValue == 0x00) && (pcmd->wIndex == USB_FTR_EXT_COMPAT_ID)) {
#endif
        if (pcmd->wLength < sizeof(Usb_Ext_Compat_ID_OS_Feature_Dscr)) {
            length = pcmd->wLength;
        } else {
            length = sizeof(Usb_Ext_Compat_ID_OS_Feature_Dscr);
        }

        /* Prepare to return Standard Device Descriptor */
        USB_Generate_EP0Data(pep0state, pcmd, &gUsbDevice.os_feature_desc, (int32_t)length);
    } else {
        bError = true;
    }

    /* EP0 will stall in error case, thus no need assertion */
    {
        hal_usb_update_endpoint_0_state(HAL_USB_EP0_DRV_STATE_READ_END, bError, false);
    }
}

#endif


/* Parse EP0 requested command */
static void USB_Endpoint0_Idle (void)
{
    /* Read ep0 data */
    hal_usb_read_endpoint_fifo(0, 8, &gUsbDevice.cmd);

    //LOG_I(hal, "USB_Endpoint0_Idle\n");
    //LOG_I(hal, "    -bmRequestType:%x\n", gUsbDevice.cmd.bmRequestType);
    //LOG_I(hal, "    -bRequest     :%x\n", gUsbDevice.cmd.bRequest);
    //LOG_I(hal, "    -wValue       :%x\n", gUsbDevice.cmd.wValue);
    //LOG_I(hal, "    -wIndex       :%x\n", gUsbDevice.cmd.wIndex);
    //LOG_I(hal, "    -wLength      :%x\n", gUsbDevice.cmd.wLength);

    /* Check request type */
    switch (gUsbDevice.cmd.bmRequestType & USB_CMD_TYPEMASK) {
        case USB_CMD_STDREQ:
            /* standard request */
            USB_Stdcmd(&gUsbDevice.ep0info, &gUsbDevice.cmd);
            break;

        case USB_CMD_CLASSREQ:
            /* class specific request */
            //check PC send Data packet:  index parts
            if ((gUsbDevice.cmd.wIndex & 0xFF) > USB_MAX_INTERFACE) {
                /* error occur, stall endpoint*/
                hal_usb_update_endpoint_0_state(HAL_USB_EP0_DRV_STATE_READ_END, true, false);
                return;
            }
            if ((gUsbDevice.cmd.bmRequestType == USB_CMD_CLASSIFIN) || (gUsbDevice.cmd.bmRequestType == USB_CMD_CLASSIFOUT)) {
                if (gUsbDevice.if_info[(gUsbDevice.cmd.wIndex & 0xff)].if_class_specific_hdlr != NULL) {
                    gUsbDevice.if_info[(gUsbDevice.cmd.wIndex & 0xff)].if_class_specific_hdlr(&gUsbDevice.ep0info, &gUsbDevice.cmd);
                } else {
                    /* error occur, stall endpoint*/
                    hal_usb_update_endpoint_0_state(HAL_USB_EP0_DRV_STATE_READ_END, true, false);
                }
            } else {
                /* error occur, stall endpoint*/
                hal_usb_update_endpoint_0_state(HAL_USB_EP0_DRV_STATE_READ_END, true, false);
            }
            break;
        case USB_CMD_VENDREQ:
        default:
            /* Stall the command if a reserved request is received */
            hal_usb_update_endpoint_0_state(HAL_USB_EP0_DRV_STATE_READ_END, true, false);
            break;
    }
}


/* EP0 RX handler, called when EP0 interrupt happened and in RX state */
static void USB_Endpoint0_Rx(void)
{
    if (gUsbDevice.ep0_rx_handler != NULL) {
        /* called rx handler to get data*/
        gUsbDevice.ep0_rx_handler(&gUsbDevice.ep0info);
    } else {
        /* this should not happened, user should register rx handler when set EP0 into RX state */
        /* error occur, stall endpoint*/
        LOG_E(hal, "ASSERT\n");
//		hal_usb_update_endpoint_0_state(HAL_USB_EP0_DRV_STATE_READ_END, true, false);
    }
}


/* EP0 TX handler, called when EP0 interrupt happened and in TX state,
     or EP0 just translate from IDLE to TX state */
static void USB_Endpoint0_Tx(void)
{
    int32_t nBytes;


    /* Determine number of bytes to send */
    if (gUsbDevice.ep0info.nBytesLeft <= HAL_USB_MAX_PACKET_SIZE_ENDPOINT_0) {
        nBytes = gUsbDevice.ep0info.nBytesLeft;
        gUsbDevice.ep0info.nBytesLeft = 0;
    } else {
        nBytes = HAL_USB_MAX_PACKET_SIZE_ENDPOINT_0;
        gUsbDevice.ep0info.nBytesLeft -= HAL_USB_MAX_PACKET_SIZE_ENDPOINT_0;
    }

    /* Send out data */
    hal_usb_write_endpoint_fifo (0, nBytes, gUsbDevice.ep0info.pData);

    /* Update data pointer and  prepare for next transaction */
    gUsbDevice.ep0info.pData = (uint8_t *)gUsbDevice.ep0info.pData + nBytes;

    if ((nBytes < HAL_USB_MAX_PACKET_SIZE_ENDPOINT_0) || (gUsbDevice.ep0_send_one_pkt == true) || ((gUsbDevice.ep0info.no_ZLP == true) && (gUsbDevice.ep0info.nBytesLeft == 0))) {
        gUsbDevice.ep0_state = USB_EP0_IDLE;
        /* Last data, set end as true */
        hal_usb_update_endpoint_0_state(HAL_USB_EP0_DRV_STATE_WRITE_RDY, false, true);
    } else {
        hal_usb_update_endpoint_0_state(HAL_USB_EP0_DRV_STATE_WRITE_RDY, false, false);
    }
}


/* EP0 interrupt handler called by USB_HISR */
extern uint32_t hal_usb_ep0_pkt_len(void);

static void USB_Endpoint0_Hdlr(void)
{
    bool b_transaction_end;
    bool b_sent_stall;
    uint32_t nCount;

    //LOG_E(hal, "USB_Endpoint0_Hdlr\n");

    hal_usb_get_endpoint_0_status(&b_transaction_end, &b_sent_stall);

    /* Check for SentStall */
    /* SentStall && SetupEnd are impossible to occur together*/
    if (b_sent_stall == true) {
        hal_usb_update_endpoint_0_state(HAL_USB_EP0_DRV_STATE_CLEAR_SENT_STALL, false, false);
        gUsbDevice.ep0_state = USB_EP0_IDLE;
    }

    /* Check for SetupEnd */
    if (b_transaction_end == true) {
        hal_usb_update_endpoint_0_state(HAL_USB_EP0_DRV_STATE_TRANSACTION_END, false, false);
        gUsbDevice.ep0_state = USB_EP0_IDLE;
    }

    /* Call relevant routines for endpoint 0 state */
    if (gUsbDevice.ep0_state == USB_EP0_IDLE) {
        /* receive command request */
        nCount = hal_usb_ep0_pkt_len();

        if (nCount > 0) {
            /* idle state handler */
            USB_Endpoint0_Idle();
        }
    } else if (gUsbDevice.ep0_state == USB_EP0_RX) {
        /* Rx state handler */
        USB_Endpoint0_Rx();
    } else if (gUsbDevice.ep0_state == USB_EP0_RX_STATUS) {
        /* Data stage is RX, status stage is TX*/
        if (gUsbDevice.nDevState == DEVSTATE_SET_ADDRESS) {
            hal_usb_set_address(gUsbDevice.ep0info.byFAddr, HAL_USB_SET_ADDR_STATUS);
            gUsbDevice.nDevState = DEVSTATE_ADDRESS;
        }

        if (gUsbDevice.usb_test_type != HAL_USB_TEST_MODE_TYPE_NONE) {
            /* After status complete, target must transmit to test mode in 3ms */
            hal_usb_enter_test_mode(gUsbDevice.usb_test_type);
            return;
        }

        gUsbDevice.ep0_state = USB_EP0_IDLE;

        /* In case next setup followed the previous status very fast and interrupt only happens once*/
        /* receive command request */
        nCount = hal_usb_ep0_pkt_len();

        if (nCount > 0) {
            /* idle state handler */
            USB_Endpoint0_Idle();
        }
    }

    /* must use if, not else if, EP0 may enter TX state in previous IDLE state handler */
    if (gUsbDevice.ep0_state == USB_EP0_TX) {
        /* Tx state handler */
        USB_Endpoint0_Tx();
    }

//LOG_E(hal, "USB_Endpoint0_Hdlr--leave\n");

}

#endif //MTK_USB_DEMO_ENABLED
