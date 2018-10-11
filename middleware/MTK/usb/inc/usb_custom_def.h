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
 *    usb_custom_def.h
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *   This file declares usb customization definitions
 *
 * Author:
 * -------
 *  Choco Chou
 *
 *============================================================================*/

/* USB VID */
#define MTK_USB_VID						(0x0e8d)

/*
 * When the USB is detected, the LED lights up
 * */

//#define USB_DETECT_LED

#ifdef USB_DETECT_LED
#define VIBR_VOL PMIC_LDO_1P8V
#define LED_GPIO HAL_GPIO_24
#endif

/*********************************************/
/*        Device Common                      */
/*********************************************/
#define USB_DESCRIPTOR_SIZE 		512

/*********************************************/
/*             CDC ACM Buffer Size           */
/*********************************************/

#define	USB_TX_RING_BUFFER_1_SIZE			(2048)
#define	USB_TX_RING_BUFFER_2_SIZE			(2048)
#define	USB_TX_RING_BUFFER_3_SIZE			(2048)

#define	USB_RX_RING_BUFFER_1_SIZE			(2048)
#define	USB_RX_RING_BUFFER_2_SIZE			(2048)
#define	USB_RX_RING_BUFFER_3_SIZE			(2048)

#ifdef  __USB_HIGH_SPEED_COM_PORT_ENABLE__
// high speed interface RX buffer
#define USB_ACM_RX_BUFFER_NUM  					8   
#define USB_ACM_RX_BUFFER_SIZE 					3072
#else
#define USB_ACM_RX_BUFFER_NUM  					0   
#define USB_ACM_RX_BUFFER_SIZE 					0
#endif

/*********************************************/
/*        Mass Storage Buffer Size           */
/*********************************************/
#if defined(__LOW_COST_SUPPORT_ULC__)
#define USBMS_MAX_BUFFERSIZE     		(4*1024)
#elif (defined(__LOW_COST_SUPPORT_COMMON__)&&(!defined(__USB_HS_ENABLE__)))
#define USBMS_MAX_BUFFERSIZE     		(8*1024)
#else
#define USBMS_MAX_BUFFERSIZE     		(32*1024)
#endif

