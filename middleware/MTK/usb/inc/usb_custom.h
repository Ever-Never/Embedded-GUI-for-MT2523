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

#ifndef USB_CUSTOM_H
#define USB_CUSTOM_H

/* custom parameter for usb device*/
typedef struct
{
	const uint16_t 	desc_vendor;
	const uint16_t 	*manufacture_string;
	const uint8_t 	manufacture_size;
	const uint16_t 	*product_string;
	const uint8_t 	product_size;
}USB_DEVICE_PARAM;


/* custom parameter for acm device*/
typedef struct
{
	const uint16_t 	desc_product;
	const uint16_t 	*comm_interface_string;
	const uint8_t 	comm_interface_size;
	const uint16_t 	*data_interface_string;
	const uint8_t 	data_interface_size;
}USB_ACM_PARAM;

/* custom parameter for mass storage device*/
typedef struct
{
	const uint16_t 	desc_product;
	const uint8_t 	*inquire_data;
	const uint8_t 	inquire_size;
	const uint16_t 	*interface_string;
	const uint8_t 	interface_size;
}USB_MS_PARAM;

typedef const USB_DEVICE_PARAM* (*usb_get_device_param_func)(void);
typedef const USB_ACM_PARAM* (*usb_get_acm_param_func)(void);
typedef const USB_MS_PARAM* (*usb_get_ms_param_func)(void);


/* get parameter functions*/
typedef struct 
{
	usb_get_device_param_func	get_device_param_func;
	usb_get_acm_param_func		get_acm_param_func;
	usb_get_ms_param_func		get_ms_param_func;
} USB_CUSTOM_FUNC;

#endif

