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

#include "usb.h"
#include "usb_resource.h"
#include "usb_custom.h"
#include "usb_custom_def.h"

static uint8_t USB_TEST_Buffer[1024];

//__attribute__((aligned(4)))
static const uint16_t USB_Language_String[] = {
    0x0304,
    0x0409
};

static const uint8_t devdscr[] = {
    // Standard Device Descriptor
    USB_DEVDSC_LENGTH,   		//bLength
    USB_DEVICE,					//bDescriptorType
    0x10, /* USB spec rev is 1.1 */ 	//bcdUSB
    0x01, /* USB spec rev is 1.1 */	//bcdUSB
    0x00,						//bDeviceClass
    0x00,						//bDeviceSubClass
    0x00,						//bDeviceProtocol
    HAL_USB_MAX_PACKET_SIZE_ENDPOINT_0, /* HAL_USB_MAX_PACKET_SIZE_ENDPOINT_0 == 8 or 64*/ //bMaxPacketSize0
    (uint8_t)(MTK_USB_VID),				//idVendor
    (uint8_t)(MTK_USB_VID >> 8),				//idVendor
    0x00,						//idProduct
    0x00,						//idProduct
    (uint8_t)(USB_DEVDSC_BCDDEVICE_PHONE),	//bcdDevice
    (uint8_t)(USB_DEVDSC_BCDDEVICE_PHONE >> 8),	//bcdDevice
    0x00,						//iManufacturer
    0x00,						//iProduct
    0x00,						//iSerialNumber
    0x01 //USB_DEVDSC_CONFIGS			//bNumConfigurations
};

static const uint8_t cfgdscr[] = {
    /* Initialise the descriptors for main configuration 1 */
    USB_CFGDSC_LENGTH,			//bLength
    USB_CONFIG, 				//bDescriptorType
    0x00,						//wTotalLength
    0x00,						//wTotalLength
    0x00,						//bNumInterfaces
    0x01,  /*the value = (cfg index+1), select this config's number*/  //bConfigurationValue
    0x00,						//iConfiguration
    USB_CFGDSC_ATTR_NATURE, 	//bmAttributes
    USB_CFGDSC_MAXPOWER 		//bMaxPower
};

static const uint8_t dev_qual_dscr[] = {
    /* Initialise the Standard Device_Qualiifier Descriptor */
    USB_DEV_QUA_DSC_LENGTH,		//bLength
    USB_DEVICE_QUALIFIER,		//bDescriptorType
    0x00, /* USB spec rev is 2.0 */	//bcdUSB
    0x02, /* USB spec rev is 2.0 */	//bcdUSB
    0x00,						//bDeviceClass
    0x00,						//bDeviceSubClass
    0x00,						//bDeviceProtocol
    HAL_USB_MAX_PACKET_SIZE_ENDPOINT_0,//bMaxPacketSize0
    0x00,						//bNumConfigurations
    0x00						//bReserved
};

static const uint8_t other_speed_cfg_dscr[] = {
    /* Initialise the descriptors for other speed configuration */
    USB_OTHER_CFGDSC_LENGTH,	//bLength
    USB_OTHER_SPEED,			//bDescriptorType
    0x00,						//wTotalLength
    0x00,						//wTotalLength
    0x00,						//bNumInterfaces
    0x01,   /*the value = (cfg index+1), select this config's number*/ //bConfigurationValue
    0x00,						//iConfiguration
    0x00,						//bmAttributes
    0x00						//MaxPower
};


/* interface specific create and init functions */
static Usb_IfCreate_Info usb_ifcreate_tbl[USB_MAX_INTERFACE];
static uint8_t usb_ifcreate_number = 0;
/* device code */
static Usb_Device_Code usb_device_code;

/* static functions */
static void USB_Resource_Reset(void);
#ifndef __USB_COMPOSITE_NO_SERIAL__
#endif

/* Register class specific create functions, class specific create function should create interface descriptor */
void USB_Register_CreateFunc(char *if_name, usb_create_if_func_ptr if_create_func, usb_void_func if_init_func,
                             usb_void_func if_enable_func, usb_speed_if_func_ptr if_speed_func, usb_void_func if_resume_func)
{
    if ((usb_ifcreate_number >= USB_MAX_INTERFACE) || (if_create_func == NULL) || (if_init_func == NULL) || (if_enable_func == NULL) || (if_speed_func == NULL)) {
        LOG_E(hal, "ASSERT\n");
    }

    usb_ifcreate_tbl[usb_ifcreate_number].if_name_ptr = if_name;
    usb_ifcreate_tbl[usb_ifcreate_number].if_create_func = if_create_func;
    usb_ifcreate_tbl[usb_ifcreate_number].if_init_func = if_init_func;
    usb_ifcreate_tbl[usb_ifcreate_number].if_enable_func = if_enable_func;
    usb_ifcreate_tbl[usb_ifcreate_number].if_speed_func = if_speed_func;
    usb_ifcreate_tbl[usb_ifcreate_number].if_resume_func = if_resume_func;
    usb_ifcreate_number++;
}


/* Register class specific device and product code */
void USB_Register_Device_Code(uint8_t device_code, uint8_t subclass_code,
                              uint8_t protocol_code, uint16_t product_code)
{
    if (usb_device_code.b_registerd == true) {
        LOG_E(hal, "ASSERT\n");
    }

    usb_device_code.device_code = device_code;
    usb_device_code.subclass_code = subclass_code;
    usb_device_code.protocol_code = protocol_code;
    usb_device_code.product_code = product_code;
    usb_device_code.b_registerd = true;
}


/* Deregister class specific create functions,class specific device and product code should be called when cable plug out */
void USB_DeRegister_CreateFunc(void)
{
    usb_ifcreate_number = 0;
    usb_device_code.b_registerd = false;
}

static void USB_Check_Ep_Number()
{
    if ((gUsbDevice.resource_ep_intr_tx_number > USB_MAX_EP_INTR_TX)
            || (gUsbDevice.resource_ep_bulk_tx_number > USB_MAX_EP_BULK_TX)
            || ((gUsbDevice.resource_ep_bulk_tx_number + gUsbDevice.resource_ep_intr_tx_number) > HAL_USB_MAX_NUMBER_ENDPOINT_TX)
            || (gUsbDevice.resource_ep_bulk_rx_number > USB_MAX_EP_BULK_RX)) {
        LOG_E(hal, "ASSERT\n");
    }
}

static void USB_Check_Descriptor()
{
    if ((gUsbDevice.resource_interface_number > USB_MAX_INTERFACE)
            || (gUsbDevice.resource_string_number > USB_MAX_STRING)
            || (gUsbDevice.resource_iad_number > USB_MAX_IAD)) {
        LOG_E(hal, "ASSERT\n");
    }
}



/* Get interrupt TX endpoint number and endpoint info pointer */
Usb_Ep_Info *USB_Get_Intr_Ep(uint8_t *p_num)
{
    /* Interrupt number assign from max */
    *p_num = HAL_USB_MAX_NUMBER_ENDPOINT_TX - gUsbDevice.resource_ep_intr_tx_number;
    gUsbDevice.resource_ep_intr_tx_number++;

    USB_Check_Ep_Number();

    return (&gUsbDevice.ep_intr_tx_info[gUsbDevice.resource_ep_intr_tx_number - 1]);
}


/* Get bulk TX endpoint number and endpoint info pointer */
Usb_Ep_Info *USB_Get_Bulk_Tx_Ep(uint8_t *p_num)
{
    gUsbDevice.resource_ep_bulk_tx_number++;
    USB_Check_Ep_Number();
    *p_num = gUsbDevice.resource_ep_bulk_tx_number;
    return (&gUsbDevice.ep_bulk_tx_info[gUsbDevice.resource_ep_bulk_tx_number - 1]);
}


/* get bulk RX endpoint number and endpoint info pointer */
Usb_Ep_Info *USB_Get_Bulk_Rx_Ep(uint8_t *p_num)
{
    gUsbDevice.resource_ep_bulk_rx_number++;
    USB_Check_Ep_Number();

    *p_num = gUsbDevice.resource_ep_bulk_rx_number;
    return (&gUsbDevice.ep_bulk_rx_info[gUsbDevice.resource_ep_bulk_rx_number - 1]);
}




/* Get interface number and interface info pointer */
Usb_Interface_Info *USB_Get_Interface_Number(uint8_t *p_num)
{
    gUsbDevice.resource_interface_number++;

    USB_Check_Descriptor();

    *p_num = gUsbDevice.resource_interface_number - 1;
    return (&gUsbDevice.if_info[gUsbDevice.resource_interface_number - 1]);
}


/* Get string number and point device recource_string to real string pointer */
uint8_t USB_Get_String_Number(void *usb_string)
{
    Usb_String_Dscr *string = (Usb_String_Dscr *)usb_string;
    gUsbDevice.resource_string_number++;

    USB_Check_Descriptor();

    gUsbDevice.resource_string[gUsbDevice.resource_string_number - 1] = string;
    return (gUsbDevice.resource_string_number - 1);
}

Usb_IAD_Dscr *USB_Get_IAD_Number(void)
{
    gUsbDevice.resource_iad_number++;
    USB_Check_Descriptor();
    return (&gUsbDevice.iad_desc[gUsbDevice.resource_iad_number - 1]);
}

/* Create all descriptors, including device and class specific */
void USB_Software_Create(void)
{
    uint32_t  index_if = 0;
    uint32_t  index_ep;
    uint32_t  if_len = 0;
    /* initial resource number */
    USB_Resource_Reset();
    memcpy(&(gUsbDevice.devdscr), devdscr, USB_DEVDSC_LENGTH);
    memcpy(&(gUsbDevice.cfgdscr), cfgdscr, USB_CFGDSC_LENGTH);

    /* string descriptor */
    USB_Get_String_Number((void *)USB_Language_String);   /*  index 0 */

    /* check class has registered create function and device code */
    if ((usb_ifcreate_number == 0) || (usb_device_code.b_registerd == false)) {
        LOG_E(hal, "ASSERT\n");
    }

    /* call the create function for each class */
    for (index_if = 0; index_if < usb_ifcreate_number; index_if++) {
        if (usb_ifcreate_tbl[index_if].if_create_func != NULL) {
            usb_ifcreate_tbl[index_if].if_create_func(usb_ifcreate_tbl[index_if].if_name_ptr);
        }

        if ((gUsbDevice.if_info[index_if].ifdscr_size == 0) || (usb_ifcreate_tbl[index_if].if_create_func == NULL)) {
            LOG_E(hal, "ASSERT\n");
        }
    }

    /* Initialise the Standard Device Descriptor */
    /* Get IP version */
    gUsbDevice.devdscr.bcdUSB = 0x0200; /* USB spec rev is 2.0 */
    gUsbDevice.devdscr.bDeviceClass = usb_device_code.device_code;
    gUsbDevice.devdscr.bDeviceSubClass = usb_device_code.subclass_code;
    gUsbDevice.devdscr.bDeviceProtocol = usb_device_code.protocol_code;
    gUsbDevice.devdscr.idProduct = usb_device_code.product_code;

    /* For PC side tool to differentiate phone or modem card */
    gUsbDevice.devdscr.bcdDevice = USB_DEVDSC_BCDDEVICE_PHONE;
    gUsbDevice.devdscr.iManufacturer = USB_Get_String_Number((void *)gUsbDevice.device_param->manufacture_string);
    gUsbDevice.devdscr.iProduct = USB_Get_String_Number((void *)gUsbDevice.device_param->product_string);
    gUsbDevice.devdscr.iSerialNumber = 0x00; /* No serial number */

    /* Initialise the descriptors for main configuration 1 */
    gUsbDevice.cfgdscr.bNumInterfaces = gUsbDevice.resource_interface_number;

    if (gUsbDevice.remoteWk == true) {
        gUsbDevice.cfgdscr.bmAttributes |= USB_CFGDSC_ATTR_REMOTEWAKEUP;
    }

    /* Configuration length */
    for (index_if = 0; index_if < gUsbDevice.resource_iad_number; index_if++) {
        if_len += gUsbDevice.iad_desc[index_if].bLength;
    }

    for (index_if = 0; index_if < gUsbDevice.resource_interface_number; index_if++) {
        if_len += gUsbDevice.if_info[index_if].ifdscr_size;

        for (index_ep = 0; index_ep < gUsbDevice.if_info[index_if].ifdscr.stdif.bNumEndpoints; index_ep++) {
            if_len += gUsbDevice.if_info[index_if].ep_info[index_ep]->epdscr_size;
        }
    }

    gUsbDevice.cfgdscr.wTotalLength = (USB_CFGDSC_LENGTH + if_len);
    /* allocate configuration descriptor memory */
    if (gUsbDevice.conf == NULL) {
        //gUsbDevice.conf = (uint8_t *)USB_Get_Memory(1024);//(uint8_t *)USB_TEST_Buffer;
        gUsbDevice.conf = (uint8_t *)USB_TEST_Buffer;
    }

    /* Initialise the Standard Device_Qualiifier Descriptor */
    memcpy(&(gUsbDevice.dev_qual_dscr), dev_qual_dscr, USB_DEV_QUA_DSC_LENGTH);

    gUsbDevice.dev_qual_dscr.bDeviceClass = usb_device_code.device_code;
    gUsbDevice.dev_qual_dscr.bDeviceSubClass = usb_device_code.subclass_code;
    gUsbDevice.dev_qual_dscr.bDeviceProtocol = usb_device_code.protocol_code;
    gUsbDevice.dev_qual_dscr.bNumConfigurations = gUsbDevice.devdscr.bNumConfigurations;

    /* Initialise the descriptors for other speed configuration */
    memcpy(&(gUsbDevice.other_speed_cfg_dscr), other_speed_cfg_dscr, USB_OTHER_CFGDSC_LENGTH);

    gUsbDevice.other_speed_cfg_dscr.wTotalLength = gUsbDevice.cfgdscr.wTotalLength;
    gUsbDevice.other_speed_cfg_dscr.bNumInterfaces = gUsbDevice.cfgdscr.bNumInterfaces;
    gUsbDevice.other_speed_cfg_dscr.iConfiguration = 0;
    gUsbDevice.other_speed_cfg_dscr.bmAttributes = gUsbDevice.cfgdscr.bmAttributes;
    gUsbDevice.other_speed_cfg_dscr.MaxPower = gUsbDevice.cfgdscr.bMaxPower;
}


void USB_Software_Speed_Init(bool b_other_speed)
{
    uint32_t  index_if = 0;
    uint32_t  index_ep;
    uint32_t  index_iad;
    uint32_t  Conf_index;
    uint32_t  index;
    if ((usb_ifcreate_number == 0) || (gUsbDevice.conf == NULL)) {
        LOG_E(hal, "ASSERT\n");
    }

    /* decide EP is high speed or full speed descriptors */
    for (index = 0; index < usb_ifcreate_number; index++) {
        if (usb_ifcreate_tbl[index].if_speed_func != NULL) {
            usb_ifcreate_tbl[index].if_speed_func(b_other_speed);
        } else {
            LOG_E(hal, "ASSERT\n");
        }
    }

    /* re-construct configuration descriptor */
    if (b_other_speed == false) {
        memcpy((uint8_t *)(gUsbDevice.conf), (uint8_t *)&gUsbDevice.cfgdscr, USB_CFGDSC_LENGTH);
        Conf_index = USB_CFGDSC_LENGTH;
    } else {
        memcpy((uint8_t *)(gUsbDevice.conf), (uint8_t *)&gUsbDevice.other_speed_cfg_dscr, USB_OTHER_CFGDSC_LENGTH);
        Conf_index = USB_OTHER_CFGDSC_LENGTH;
    }

    if (gUsbDevice.resource_iad_number == 0) {
        /* No IAD, not VIDEO nor composite, either */
        for (index_if = 0; index_if < gUsbDevice.resource_interface_number; index_if++) {
            memcpy((uint8_t *)(gUsbDevice.conf + Conf_index), (uint8_t *)&gUsbDevice.if_info[index_if].ifdscr.stdif,
                   gUsbDevice.if_info[index_if].ifdscr_size);
            Conf_index += gUsbDevice.if_info[index_if].ifdscr_size;

            for (index_ep = 0; index_ep < gUsbDevice.if_info[index_if].ifdscr.stdif.bNumEndpoints; index_ep++) {
                memcpy((uint8_t *)(gUsbDevice.conf + Conf_index), (uint8_t *) & (gUsbDevice.if_info[index_if].ep_info[index_ep]->epdesc.stdep),
                       gUsbDevice.if_info[index_if].ep_info[index_ep]->epdscr_size);
                Conf_index += gUsbDevice.if_info[index_if].ep_info[index_ep]->epdscr_size;
            }
        }
    } else {
        for (index_iad = 0; index_iad < gUsbDevice.resource_iad_number; index_iad++) {
            memcpy((uint8_t *)(gUsbDevice.conf + Conf_index), (uint8_t *)&gUsbDevice.iad_desc[index_iad], gUsbDevice.iad_desc[index_iad].bLength);
            Conf_index += gUsbDevice.iad_desc[index_iad].bLength;

            for (index_if = gUsbDevice.iad_desc[index_iad].bFirstInterface;
                    index_if < (gUsbDevice.iad_desc[index_iad].bFirstInterface + gUsbDevice.iad_desc[index_iad].bInterfaceCount); index_if++) {
                memcpy((uint8_t *)(gUsbDevice.conf + Conf_index), (uint8_t *)&gUsbDevice.if_info[index_if].ifdscr.stdif,
                       gUsbDevice.if_info[index_if].ifdscr_size);
                Conf_index += gUsbDevice.if_info[index_if].ifdscr_size;

                for (index_ep = 0; index_ep < gUsbDevice.if_info[index_if].ifdscr.stdif.bNumEndpoints; index_ep++) {
                    memcpy((uint8_t *)(gUsbDevice.conf + Conf_index), (uint8_t *) & (gUsbDevice.if_info[index_if].ep_info[index_ep]->epdesc.stdep),
                           gUsbDevice.if_info[index_if].ep_info[index_ep]->epdscr_size);
                    Conf_index += gUsbDevice.if_info[index_if].ep_info[index_ep]->epdscr_size;
                }
            }
        }

    }
    if (Conf_index != gUsbDevice.cfgdscr.wTotalLength) {
        LOG_E(hal, "ASSERT\n");
        while (1);
    }
}



/* init EP of each interface (class specific) */
void USB_Software_Init(void)
{
    uint32_t index;


    for (index = 0; index < usb_ifcreate_number; index++) {
        if (usb_ifcreate_tbl[index].if_init_func != NULL) {
            usb_ifcreate_tbl[index].if_init_func();
        } else {
            LOG_E(hal, "ASSERT\n");
        }
    }
}

/* resume EP of each interface (class specific) */
void USB_Software_Resume(void)
{
    uint32_t index;
    for (index = 0; index < usb_ifcreate_number; index++) {
        if (usb_ifcreate_tbl[index].if_resume_func != NULL) {
            usb_ifcreate_tbl[index].if_resume_func();
        }
    }
}


/* init EP of each interface (class specific) */
void USB_Software_Enable(void)
{
    uint32_t index;


    for (index = 0; index < usb_ifcreate_number; index++) {
        if (usb_ifcreate_tbl[index].if_enable_func != NULL) {
            usb_ifcreate_tbl[index].if_enable_func();
        } else {
            LOG_E(hal, "ASSERT\n");
        }
    }
}

/* Reset the resource maintain in this file */
static void USB_Resource_Reset(void)
{
    uint32_t temp_addr, temp_addr2;
    temp_addr = (uint32_t)&gUsbDevice.resource_ep_bulk_tx_number;
    temp_addr2 = (uint32_t)&gUsbDevice.resource_iad_number;


    memset(&gUsbDevice.resource_ep_bulk_tx_number, 0, temp_addr2 - temp_addr);
}

#endif //MTK_USB_DEMO_ENABLED
