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
 *    usbms_state.h
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *    This file intends for usb mass storage state machine definitions
 *
 * Author:
 * -------
 *   Jensen Hu
 *   CJung Chen  
 *
 *****************************************************************************/
 
#ifndef USBMS_STATE_H
#define USBMS_STATE_H

#include "kal_data_types.h"

#define USBMS_DIR_OUT		0x00
#define USBMS_DIR_IN		0x80

typedef enum 
{
	USB_MS_ERROR_OK = 0,
	USB_MS_ERROR_RX_GETNEXTCMD,
	USB_MS_ERROR_RX_ACK,
	USB_MS_ERROR_RX_TX,
	USB_MS_ERROR_TX_IDLE,
	USB_MS_ERROR_TX_RX
} USB_MS_ERROR;

typedef enum 
{
	USB_MS_READ = 0,
	USB_MS_WRITE
} USB_MS_DIRECTION;

typedef enum
{
	MSG_ID_USB_MSDRV_RESET_IND = 0, 			/* mass storage messages */
	MSG_ID_USB_MSDRV_REC_DONE_CONF,
	MSG_ID_USB_MSDRV_TRX_DONE_CONF,
	MSG_ID_USB_MSDRV_CLEAR_STALL_REQ,
	MSG_ID_USB_MSDRV_REMOUNT_REQ
} USB_Ms_State;

extern void USB_Ms_State_Main(USB_Ms_State event, usb_ms_rec_done_conf_struct *local_para_ptr);

#if 0//def __P_PROPRIETARY_COPYRIGHT__
extern void USB_Ms_Sort_Protect_Items(uint32_t* data_item, uint32_t* second_data_item);
#endif

#endif // USBMS_STATE_H

