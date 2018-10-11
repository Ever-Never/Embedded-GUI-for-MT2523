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
 *    usbms_drv.h
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *   This file intends for usb MASS STORAGE class driver definitions
 *
 * Author:
 * -------
 *   Jensen Hu
 *   CJung Chen   
 *
 *****************************************************************************/
 
#ifndef USBMS_DRV_H
#define USBMS_DRV_H

#include "kal_data_types.h"
#include "usb_custom.h"
#include "usb_msdisk.h"
#include "usb.h"
#include "usb_custom_def.h"

/***********************************************
	ms class specific command definition
************************************************/ 

#define USB_MS_DEVICE_CODE				0x00
#define USB_MS_SUBCLASS_CODE			0x00
#define USB_MS_PROTOCOL_CODE			0x00
#define USB_MS_INTERFACE_CLASS_CODE 	0x08
#define USB_MS_INTERFACE_SUBCLASS_CODE 	0x06
#define USB_MS_INTERFACE_SUBCLASS_CODE_2 	0x05
#define USB_MS_INTERFACE_PROTOCOL_CODE 	0x50

/*Class Requset Code, EP0 command*/
#define USB_MS_RESET         			0xff
#define USB_MS_GETMAXLUN     			0xfe

/*Mass storage command*/
#define USBMS_FORMAT_UNIT				0x04
#define USBMS_INQUIRY					0x12
#define USBMS_MODE_SENSE   				0x5a
#define USBMS_MODE_SENSE6   			0x1a
#define USBMS_MODE_SELECT     				0x55
#define USBMS_MODE_SELECT6     			0x15
#define USBMS_PREVALLOW_MEDIA_REMOVL	0x1e
#define USBMS_READ10 						0x28
#define USBMS_READ12 						0xa8
#define USBMS_READ_FORMATCAPACITY		0x23
#define USBMS_REQUESTSENSE				0x03
#define USBMS_START_STOP_UNIT			0x1b
#define USBMS_TEST_UNIT_READY			0x00
#define USBMS_VERIFY					0x2f
#define USBMS_WRITE10			   			0x2a
#define USBMS_WRITE12			   			0xaa
#define USBMS_READ_CAPACITY   			0x25
#define USBMS_SYNCHRONIZE_CACHE_10   			0x35


/*CD-ROM command*/
#define USBMS_READ_TOC  				0x43
#define USBMS_READ_DISC_INFO			0X51

/*Vendor Command*/
#define USBMS_VENDOR_READ10				0xD0
#define USBMS_VENDOR_WRITE10			0xD1
#define USBMS_VENDOR_CMD				0xF0
#define USBMS_VENDOR_CMD_SUB_SET		0x01
#define USBMS_VENDOR_CMD_SUB_GET		0x02
#define USBMS_VENDOR_CMD_SUB_META		0x03
#define USBMS_VENDOR_CMD_SUB_SET_WIN	0x01
#define USBMS_VENDOR_CMD_SUB_SET_MAC	0x02
#define USBMS_VENDOR_CMD_SUB_SET_LINUX	0x03



/***********************************************
	implement definition
************************************************/

#define USB_MS_EP_NUMBER				2 

#define MAX_DISK_DRVER              	4

#define USBMS_DMA_TIMEOUT 		480

#define USBMS_MAX_SINGLE_BUFFERSIZE     (USBMS_MAX_BUFFERSIZE/2)

#define USBMS_RX_MAX_SECTOR         	(USBMS_MAX_SINGLE_BUFFERSIZE/512)
#define USBMS_TX_MAX_SECTOR          	(USBMS_MAX_SINGLE_BUFFERSIZE/512)

#define USBMS_READ_FAIL_TIMEOUT         50
#define USBMS_VENDOR_CMD_TIMEOUT         50
#define USBMS_UNKNOW_CMD_TIMEOUT         500

#define	USBMS_INVALID_MAX_LUN			0xFF

/***********************************************
	ms class specific descriptor structure
************************************************/

typedef struct
{
	uint32_t dCBWSignature;
	uint32_t dCBWTag;
	uint32_t dCBWDataTransferLength;
	uint8_t  bmCBWFlags;
	uint8_t  bCBWLUN;
	uint8_t  bCBWCBLength;
	uint8_t  CBWCB[16];
} UsbMs_CBW;


typedef struct
{
	uint32_t dCSWSignature;
	uint32_t dCSWTag;
	uint32_t dCSWDataResidue;    
	uint8_t  bmCSWStatus;        	
	uint8_t  padding[3];			/* padding to multiple of 4*/
} UsbMs_CSW;


/***********************************************
	implement enum and structure 
************************************************/

/*Mass storage state*/
typedef enum 
{
	USBMS_IDLE = 0,  		/*Receive Command*/
	USBMS_RX,  		/*Receive Data*/
	USBMS_TX,  		/*Transmit Data*/
	USBMS_ACK  		/*Transmit Data*/
}USBMS_STATE;


/* read capacity command structure */
typedef struct
{
	uint32_t LastBA;	/*depend on the target*/
	uint32_t BlkLen;	/*512*/
} Read_Capacity_Info;     


/* maintain read/write status */
typedef struct
{
	uint32_t	LBA;       /*Logical block address*/
	uint32_t	BlkLen;    /*512*/
	uint32_t	rwindex;   /*if (rwindex==BlkLen) transation done.*/
	uint32_t	msdc_rwindex;
	bool	rw_error_status;
	uint8_t	rw_buffer_index;
} UsbMs_RW_Cmd;  


typedef struct
{
	uint8_t	ms_buffer[2][USBMS_MAX_SINGLE_BUFFERSIZE];
}Usb_Disk_Buffer;


typedef struct
{
	USBMS_STATE			nState;
	bool 			cbw_is_vaild;
	bool 			ms_is_reset;				/* USB ep0 receive reset cmd */
	bool			usb_reset;					/* USB receive reset signal */
	bool			ms_read_write_reset;        /* USB bus reset vaule=True, After READ/WRITE10(12) = False */
	uint8_t   		max_LUN;
	uint8_t			current_LUN;
	bool 			is_send_usb_reset_ilm; /* avoid to send reset messages too many times */
	bool			power_off_state[MAX_DISK_DRVER];
	uint8_t  		    CSWStatusError;
	uint32_t			CSWDataResidue;
	
	Usb_EpBIn_Status	*txpipe;
	Usb_EpBOut_Status	*rxpipe;
	uint32_t			CBWdata[32/4];
	UsbMs_RW_Cmd      	rw_cmd;

	Usb_Ep_Info			*ms_ep_in_info;
	Usb_Ep_Info			*ms_ep_out_info;
	Usb_Interface_Info	*ms_if_info;
	uint8_t   		*sensePtr;
	/* buffer for read/write operation*/
	Usb_Disk_Buffer		*disk_buffer;
	/* customizable variables*/
   	const USB_MS_PARAM	*ms_param;
#if 0//def __P_PROPRIETARY_COPYRIGHT__
	bool			b_read_protect;
	uint32_t			*read_protect_items;
	uint32_t			*read_protect_second_items;
	uint32_t			*read_protect_index;			
	uint32_t			*read_protect_second_index;
	uint32_t			read_protect_num;	
	USB_STORAGE_DEV_TYPE	read_protect_dev_type;
#endif
} UsbMs_Struct;


/***********************************************
	function and global variable
************************************************/
extern UsbMs_Struct g_UsbMS;

extern void USB_Ms_BuildRx(void *addr,uint32_t len);
extern void USB_Ms_BuildTx(void *addr, uint32_t len);

extern void USB_Ms_If_Create(void *ifname);
extern void USB_Ms_If_Reset(void);
extern void USB_Ms_If_Resume(void);

extern void USB_Ms_If_Enable(void);
extern void USB_Ms_If_Speed_Reset(bool b_other_speed);

extern void USB_Init_Ms_Status(void);
extern void USB_Release_Ms_Status(void);

#endif /* USBMS_DRV_H */

