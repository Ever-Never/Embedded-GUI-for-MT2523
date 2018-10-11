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


#include "usb_custom.h"


/************************************************************
	external variables
*************************************************************/

/************************************************************
	device descriptor parameters
*************************************************************/

static const uint16_t USB_MANUFACTURER_STRING[] = {
    0x031a,
    'M',
    'e',
    'd',
    'i',
    'a',
    'T',
    'e',
    'k',
    ' ',
    'I',
    'n',
    'c'
};

static const uint16_t USB_PRODUCT_STRING[] = {
    0x0310,
    'P',
    'r',
    'o',
    'd',
    'u',
    'c',
    't'
};


static const USB_DEVICE_PARAM usb_device_param = {
    0x0e8d,				/* vendor id */
    USB_MANUFACTURER_STRING,
    sizeof(USB_MANUFACTURER_STRING) / sizeof(uint16_t),
    USB_PRODUCT_STRING,
    sizeof(USB_PRODUCT_STRING) / sizeof(uint16_t)
};

/************************************************************
	CDC ACM parameters
*************************************************************/

static const uint16_t USB_ACM_COMM_INTERFACE_STRING[] = {
    0x031A,
    'C',
    'O',
    'M',
    '(',
    'c',
    'o',
    'm',
    'm',
    '_',
    'i',
    'f',
    ')'
};

static const uint16_t  USB_ACM_DATA_INTERFACE_STRING[] = {
    0x031A,
    'C',
    'O',
    'M',
    '(',
    'd',
    'a',
    't',
    'a',
    '_',
    'i',
    'f',
    ')'
};
static const USB_ACM_PARAM usb_acm_param = {
    0x0003,				/* CDC ACM Product ID */
    USB_ACM_COMM_INTERFACE_STRING,
    sizeof(USB_ACM_COMM_INTERFACE_STRING) / sizeof(uint16_t),
    USB_ACM_DATA_INTERFACE_STRING,
    sizeof(USB_ACM_DATA_INTERFACE_STRING) / sizeof(uint16_t),
};

/*USB mass storage customize*/

/* Inquire data explanation
   The length byte(Byte 4) should be always not changed, the mass storage spec define it
   Byte 8 to 15 is Vendor Information
   Byte 16 to 31 is Product Identification
   Byte 32 to 35 is Product Revision Level */

ATTR_RWDATA_IN_NONCACHED_RAM uint8_t INQUIRE_DATA[] = {
    0x00,
    0x80,
    0x00,
    0x01,
    0x1f, /*length*/
    0x00,
    0x00,
    0x00,
    'M',  /*Vendor Identification*/
    'E',
    'D',
    'I',
    'A',
    'T',
    'E',
    'K',
    ' ', /*Product Identification*/
    'F',
    'L',
    'A',
    'S',
    'H',
    ' ',
    'D',
    'I',
    'S',
    'K',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ', /*Product Revision Level*/
    ' ',
    ' ',
    ' '
};

static const uint16_t  USB_MS_INTERFACE_STRING[] = {
    0x031c,
    'M',
    'a',
    's',
    's',
    ' ',
    'S',
    't',
    'o',
    'r',
    'a',
    'g',
    'e',
    ' '
};


static const USB_MS_PARAM usb_ms_param = {
    0x0002,			/* Mass storage Product ID */
    INQUIRE_DATA,
    sizeof(INQUIRE_DATA) / sizeof(uint8_t),
    USB_MS_INTERFACE_STRING,
    sizeof(USB_MS_INTERFACE_STRING) / sizeof(uint16_t)
};





/************************************************************
	customization functinos
*************************************************************/

/* get general USB device parameter function*/
const USB_DEVICE_PARAM *USB_GetDeviceParam(void)
{
    return (&usb_device_param);
}

/* get CDC ACM USB device parameter function*/
const USB_ACM_PARAM *USB_GetAcmParam(void)
{
    return (&usb_acm_param);
}

/* get mass storage USB device parameter function*/
const USB_MS_PARAM *USB_GetMsParam(void)
{
    return (&usb_ms_param);
}


/* get USB custom parameter function*/
static const USB_CUSTOM_FUNC USB_CustomFunc = {
    USB_GetDeviceParam,
    USB_GetAcmParam,
    USB_GetMsParam
};

const USB_CUSTOM_FUNC *USB_GetCustomFunc(void)
{
    return (&USB_CustomFunc);
}



#endif //MTK_USB_DEMO_ENABLED

