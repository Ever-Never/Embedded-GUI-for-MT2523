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
 *    usbacm_adap.h
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *  This file intends for usb adaption layer for UART API definitions
 *
 * Author:
 * -------
 *   Jensen Hu
 *   CJung Chen 
 *
 *============================================================================
 ****************************************************************************/
 
#ifndef USBACM_ADAP_H
#define USBACM_ADAP_H

typedef enum 
{
	CDC_ONLY_PUSH = 0,  	//Only push buffer  ( initial sate: prepare buffer for USB )
	CDC_ONLY_GET,   	//  Only get data   ( there might be no FLC buffer anymore )
	CDC_GET_DATA,   	//  normal state	( get data & push another buffer )
	CDC_POP_BUFF    	//  Get empty buffer  (leave RNDIS mode „³ release FLC buffer )
}USB_Requ_type;


extern void USB2UART_init(void);
extern void USB2UART_Clear_Tx_Buffer(uint8_t port);
extern void USB2UART_Clear_Rx_Buffer(uint8_t port);


extern void USB2UART_ClrRxBuffer(uint8_t port);
extern uint16_t USB2UART_GetRxAvail(uint8_t port);
extern uint16_t USB2UART_GetBufAvail(BUFFER_INFO *buf_info);
extern uint16_t USB2UART_GetBytes(uint8_t port, uint8_t *buffaddr, uint16_t length);
extern void USB2UART_ClrTxBuffer(uint8_t port);
extern uint16_t USB2UART_GetTxRoomLeft(uint8_t port);
extern uint16_t USB2UART_PutBytes(uint8_t port, uint8_t *buffaddr, uint16_t length);
extern uint8_t USB2UART_GetByte_Polling(uint8_t port);
extern void USB2UART_PutData_Polling(uint8_t port, uint8_t *Buffaddr, uint16_t Length);
extern uint16_t USB2UART_GetBuffRoomLeft(BUFFER_INFO *buf_info);
extern uint16_t USB2UART_GetTxRoomLeft(uint8_t port);

#endif /* USBACM_ADAP_H */

