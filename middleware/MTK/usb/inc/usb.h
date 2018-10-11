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

#ifndef USB_H
#define USB_H

#include "hal_usb.h"
#include "usb_custom.h"
#include "usb_comm.h"
#include "kal_data_types.h"

/***********************************************
	standard command definition
************************************************/

/* Standard Request Codes */
#define USB_GET_STATUS			0x00
#define USB_CLEAR_FEATURE		0x01
#define USB_SET_FEATURE			0x03
#define USB_SET_ADDRESS			0x05
#define USB_GET_DESCRIPTOR		0x06
#define USB_SET_DESCRIPTOR		0x07
#define USB_GET_CONFIGURATION	0x08
#define USB_SET_CONFIGURATION	0x09
#define USB_GET_INTERFACE		0x0A
#define USB_SET_INTERFACE		0x0B
#define USB_SYNCH_FRAME			0x0C

/* Vendor Request Codes */
#define USB_GET_MS_DESCRIPTOR		0xCC

/* Command bit fields */
#define USB_CMD_DATADIR			0x80

/* Request Type Field */
#define USB_CMD_TYPEMASK		0x60
#define USB_CMD_STDREQ			0x00
#define USB_CMD_CLASSREQ		0x20
#define USB_CMD_VENDREQ			0x40
#define USB_CMD_STDDEVIN		0x80
#define USB_CMD_STDDEVOUT		0x00
#define USB_CMD_STDIFIN			0x81
#define USB_CMD_STDIFOUT		0x01
#define USB_CMD_STDEPIN			0x82
#define USB_CMD_STDEPOUT		0x02
#define USB_CMD_CLASSIFIN		0xA1
#define USB_CMD_CLASSIFOUT		0x21
#define USB_CMD_VENDDEVIN		0xC0
#define USB_CMD_VENDDEVOUT	0x40


/* Standard command descriptor type*/
#define USB_DEVICE				0x01
#define USB_CONFIG				0x02
#define USB_STRING				0x03
#define USB_INTERFACE			0x04
#define USB_ENDPOINT			0x05
#define USB_DEVICE_QUALIFIER 		0x06
#define USB_OTHER_SPEED			0x07
#define USB_INTERFACE_POWER		0x08
#define USB_OTG_DESC			0x09
#define USB_INTERFACE_ASSOCIATION		0x0B

#define USB_CMD_DESCMASK		0xFF00
#define USB_CMD_DEVICE			(USB_DEVICE<<8)
#define USB_CMD_CONFIG			(USB_CONFIG<<8)
#define USB_CMD_STRING			(USB_STRING<<8)
#define USB_CMD_INTERFACE		(USB_INTERFACE<<8)
#define USB_CMD_ENDPOINT		(USB_ENDPOINT<<8)
#define USB_CMD_DEVICE_QUALIFIER		(USB_DEVICE_QUALIFIER<<8)
#define USB_CMD_OTHER_SPEED			(USB_OTHER_SPEED<<8)
#define USB_CMD_INTERFACE_POWER		(USB_INTERFACE_POWER<<8)

/* Standard Device Feature Selectors */
#define USB_FTR_DEVREMWAKE			0x0001
#define USB_FTR_EPHALT				0x0000
#define USB_FTR_B_HNP_ENB			0x0003
#define USB_FTR_A_HNP_SUPPORT		0x0004
#define USB_FTR_A_ALT_HNP_SUPPORT	0x0005
#define USB_FTR_TEST_MODE				0x0002

/* Vendor OS feature descriptor, feature index */
#define USB_FTR_GENRE					0x0001
#define USB_FTR_EXT_COMPAT_ID			0x0004
#define USB_FTR_EXT_PROPERTIES		0x0005


/* USB test mode */
#define USB_TEST_J						0x01
#define USB_TEST_K						0x02
#define USB_TEST_SE0_NAK				0x03
#define USB_TEST_PACKET				0x04

/***********************************************
	standard descriptor definition
************************************************/
/* Scriptor length */
#define USB_DEVDSC_LENGTH			18
#define USB_CFGDSC_LENGTH			9
#define USB_IFDSC_LENGTH			9
#define USB_MAX_IFDSC_LENGTH		258	/* this will change depend on different class interface */
						/*descriptor requirment*/
						/* 258 is for video streaming class interface */	
#define USB_EPDSC_LENGTH			7
#define USB_OTGDSC_LENGTH			3
#define USB_MAX_CLASS_EPDESC_LENGTH	12	/* this will change depend on different class interface */
						/*descriptor requirment*/
						/* 12 is for video control interrupt ep */	
#define USB_IAD_LENGTH				8
#define USB_DEV_QUA_DSC_LENGTH		10
#define USB_OTHER_CFGDSC_LENGTH			9
#define USB_MAX_OS_STRING_LENGTH			18
#define USB_MAX_OS_FEATURE_DESC_LENGTH		40


/* Endpoint direction */
#define USB_EP_DIR				0x80
#define USB_EP_DIR_IN			0x80
#define USB_EP_DIR_OUT			0x00

/* Endpoint transfer types */
#define USB_EP_TFMASK			0x03
#define USB_EP_CONTROL			0x00
#define USB_EP_ISO				0x01
#define USB_EP_BULK				0x02
#define USB_EP_INTR				0x03

/* ISO Synchronization types */
#define USB_ISO_NO_SYNCHRONIZATION		0x00
#define USB_ISO_ASYNCHRONOUS				0x01
#define USB_ISO_ADAPTIVE					0x02
#define USB_ISO_SYNCHRONOUS				0x03

/* ISO Usage types */
#define USB_ISO_DATA						0x00
#define USB_ISO_FEEDBACK					0x01
#define USB_ISO_IMPLICIT_FEEDBACK			0x02

/*****Descriptor Element*****/
/* Define values for Standard Device Descriptor */
/* For Device descriptor */

/* bcdDevice value */
#define USB_DEVDSC_BCDDEVICE_PHONE			0x0100   /* Release number of USB device */
#define USB_DEVDSC_BCDDEVICE_MODEM_CARD			0x0200   /* Release number of USB modem card device for ECMT */
#define USB_DEVDSC_BCDDEVICE_MODEM_CARD_FOR_NDIS	0x0300   /* Release number of USB modem card device for NDIS*/

/* MS OS : Device class code for composite device */
#define USB_COMPOSITE_DEVICE_CODE				0xEF
#define USB_COMPOSITE_SUBCLASS_CODE				0x02
#define USB_COMPOSITE_PROTOCOL_CODE				0x01

/* MAC OS : Device class code for composite device */
#define USB_MAC_COMPOSITE_DEVICE_CODE				0x02
#define USB_MAC_COMPOSITE_SUBCLASS_CODE				0x00
#define USB_MAC_COMPOSITE_PROTOCOL_CODE				0x00

/* Device class code for without IAD composite device */
#define USB_COMPOSITE_NO_IAD_DEVICE_CODE				0x00
#define USB_COMPOSITE_NO_IAD_SUBCLASS_CODE				0x00
#define USB_COMPOSITE_NO_IAD_PROTOCOL_CODE				0x00


#define USB_DEVDSC_CONFIGS    			USB_MAX_CONFIG // USB_DEVDSC_CONFIGS   -->  USB_GetChargeCurrentTableSize() 


/*For Configuration descriptor*/
#define USB_CFGDSC_ATTR_NATURE			0x80
#define USB_CFGDSC_ATTR_SELFPOWER		0x40
#define USB_CFGDSC_ATTR_REMOTEWAKEUP	0x20
#define USB_CFGDSC_MAXPOWER				(500/2)

#define USB_OTG_HNP_SUPPORT				0x02
#define USB_OTG_SRP_SUPPORT				0x01


/***********************************************
	Hub class code definition
************************************************/
#define USB_HUB_DEVICE_CLASS_CODE				0x09
#define USB_HUB_DEVICE_SUBCLASS_CODE				0x00
//#define USB_HUB_DEVICE_PROTOCOL_CODE				0x00
#define USB_HUB_INTERFACE_CLASS_CODE 			0x09
#define USB_HUB_INTERFACE_SUBCLASS_CODE 			0x00
//#define USB_HUB_INTERFACE_PROTOCOL_CODE 			0x00


/***********************************************
	Implement definition
************************************************/

/* Define configuration, interface, ep number */
#define USB_MAX_CONFIG				6  /* configuration number */
#define USB_MAX_IAD					2
#define USB_MAX_INTERFACE			4  /* interface number */
#define USB_MAX_INTERFACE_ALTERNATE_NUM		1  /* interface alternate seting number */

#define USB_MAX_EP_BULK_TX		2  /* max bulk in ep number */
#define USB_MAX_EP_BULK_RX		2  /* max bulk in ep number */
#define USB_MAX_EP_INTR_TX			2  /* max interrupt ep number */
#define USB_MAX_EP_ISO_TX			1  /* max interrupt ep number */

#define USB_MAX_STRING				12 /* string number */
#define USB_MAX_EP_PER_IF			3 /* 3 is for image class */

/* serial string array size  for that unique Serial number*/
#define USB_SERIAL_STRING_LEN		16

/* define endpoint data parameters */
#define USB_EP_NODATA				-1  /* no more data for endpoint to send */


/***********************************************
	standard command structure
************************************************/

/* usb standard command structure */
typedef struct 
{
	uint8_t	bmRequestType;
	uint8_t	bRequest;
	uint16_t	wValue;
	uint16_t	wIndex;
	uint16_t	wLength;
}Usb_Command;



/***********************************************
	Standard descriptor structure
************************************************/

/* standard device descriptor */
typedef struct 
{
	uint8_t    bLength;
	uint8_t    bDescriptorType;
	uint16_t   bcdUSB;
	uint8_t    bDeviceClass;
	uint8_t    bDeviceSubClass;
	uint8_t    bDeviceProtocol;
	uint8_t    bMaxPacketSize0;
	uint16_t   idVendor;
	uint16_t   idProduct;
	uint16_t   bcdDevice;
	uint8_t    iManufacturer;
	uint8_t    iProduct;
	uint8_t    iSerialNumber;
	uint8_t    bNumConfigurations;
}Usb_Dev_Dscr;


/* standard configuration descriptor */
typedef struct 
{
	uint8_t    bLength;
	uint8_t    bDescriptorType;
	uint16_t   wTotalLength;
	uint8_t    bNumInterfaces;
	uint8_t    bConfigurationValue;
	uint8_t    iConfiguration;
	uint8_t    bmAttributes;
	uint8_t    bMaxPower;
}Usb_Cfg_Dscr;


/* standard IAD descriptor */
typedef struct 
{
	uint8_t    bLength;
	uint8_t    bDescriptorType;
	uint8_t    bFirstInterface;
	uint8_t    bInterfaceCount;
	uint8_t    bFunctionClass;
	uint8_t    bFunctionSubClass;
	uint8_t    bFunctionProtocol;
	uint8_t    iFunction;
}Usb_IAD_Dscr;


/* standard interface descriptor */
typedef struct 
{
	uint8_t    bLength;
	uint8_t    bDescriptorType;
	uint8_t    bInterfaceNumber;
	uint8_t    bAlternateSetting;
	uint8_t    bNumEndpoints;
	uint8_t    bInterfaceClass;
	uint8_t    bInterfaceSubClass;
	uint8_t    bInterfaceProtocol;
	uint8_t    iInterface;
}Usb_If_Dscr;


/* standard endpoint descriptor */
typedef struct 
{
	uint8_t    bLength;
	uint8_t    bDescriptorType;
	uint8_t    bEndpointAddress;
	uint8_t    bmAttributes;
	uint8_t    wMaxPacketSize[2];	
	uint8_t    bInterval;
}Usb_Ep_Dscr;


/* standard string descriptor */
typedef struct 
{
	uint8_t    bLength;
	uint8_t    bDescriptorType;
	uint16_t   wData[1];
}Usb_String_Dscr;


/* standard string descriptor */
typedef struct 
{
	uint8_t    bLength;
	uint8_t    bDescriptorType;
	uint8_t    bAttribute;
}Usb_Otg_Dscr;


typedef struct 
{
	uint8_t    bLength;
	uint8_t    bDescriptorType;
	uint16_t  bcdUSB;
	uint8_t    bDeviceClass;
	uint8_t    bDeviceSubClass;
	uint8_t    bDeviceProtocol;
	uint8_t    bMaxPacketSize0;
	uint8_t    bNumConfigurations;
	uint8_t    bReserved;
}Usb_Dev_Qual_Dscr;


typedef struct 
{
	uint8_t    bLength;
	uint8_t    bDescriptorType;
	uint16_t  wTotalLength;
	uint8_t    bNumInterfaces;
	uint8_t    bConfigurationValue;
	uint8_t    iConfiguration;
	uint8_t    bmAttributes;
	uint8_t    MaxPower;
}Usb_Other_Speed_Cfg_Dscr;


typedef struct
{
	uint8_t    bLength;
	uint8_t    bDescriptorType;
	uint8_t    qwSignature[14];
	uint8_t    bMS_VendorCode;
	uint8_t    bPad;
}Usb_OS_String_Dscr;


typedef struct
{
	uint32_t    dwLength;
	uint16_t    bcdVersion;
	uint16_t    wIndex;
	uint8_t     bCount;
	uint8_t     bPads_1[7];
	uint8_t     bFirstInterfaceNumber;
	uint8_t     bPads_2;
	uint8_t     compatibleID[8];
	uint8_t     subcompatibleID[8];
	uint8_t     bPads_3[6];
}Usb_Ext_Compat_ID_OS_Feature_Dscr;


/***********************************************
	implement enum and structure 
************************************************/

/* device states */
typedef enum
{
	DEVSTATE_DEFAULT = 0,
	DEVSTATE_SET_ADDRESS,
	DEVSTATE_ADDRESS,
	DEVSTATE_CONFIG
}USB_DEVICE_STATE;


/* Endpoint 0 state */
typedef enum 
{
	USB_EP0_IDLE=0,
	USB_EP0_RX,
	USB_EP0_TX,
	USB_EP0_RX_STATUS
}USB_EP0_STATE;


/* EP0 status */
typedef struct 
{
	int32_t	nBytesLeft;	/* number of bytes left to send in TX mode */
	void		*pData;		/* pointer to data to transmit/receive */
	int32_t	nBytesRecv;	/* number of bytes received in RX mode */
	uint8_t	byFAddr;	/* new function address */
	bool	no_ZLP;	/* No need ZLP when wLength == transfer size */
}Usb_Ep0_Status;


/* Bulk IN endpoint status */
typedef struct 
{
	void		*pData;		/* pointer to data buffer */
	int32_t	nBytesLeft;	/* number of bytes left to send */
	uint8_t	byEP;		/* endpoint number */
}Usb_EpBIn_Status;


/* Bulk OUT endpoint status */
typedef struct 
{
	void		*pData;		/* pointer to data buffer */
	int32_t	nBuffLen;	/* length of data buffer */
	int32_t	nBytesRecv;	/* number of bytes received */
	uint8_t	byEP;		/* endpoint number */
}Usb_EpBOut_Status;


typedef enum
{
	USB_NORMAL_BOOT = 0,
	USB_BOOT_ONE_PORT,
	USB_BOOT_TWO_PORT,
	USB_BOOT_THREE_PORT,
	USB_BOOT_FOUR_PORT,
	USB_BOOT_FIVE_PORT
//	USB_RNDIS_ONE_PORT
}USB_Comport_Boot_Status;

typedef enum
{
	USB_COMPORT_WIN = 0,
	USB_COMPORT_MAC,
	USB_COMPORT_LINUX,
}USB_Comport_OS_Driver_Status;

typedef enum
{
	USB_MASK_OWNER_COM_1 = 0,
	USB_MASK_OWNER_COM_2,
	USB_MASK_OWNER_COM_3,
	USB_MASK_OWNER_COM_4,
	USB_MASK_OWNER_COM_5,
	USB_MASK_OWNER_MS = 10,	
	USB_MASK_OWNER_ALL,
	USB_MASK_OWNER_MAX
}USB_MASK_OWNER;


/* interface create function pointer */
typedef void (*usb_create_if_func_ptr)(void *);
/* interface speed reset function pointer */
typedef void (* usb_speed_if_func_ptr)(bool bset);
/* EP0 rx interrupt handler */
typedef void (*usb_ep0_rx_ptr)(void *);
/* class specific command interrupt handler */
typedef void (*usb_class_specific_handler_ptr)(Usb_Ep0_Status *, Usb_Command*);
/* endpoint interrupt handler */
typedef void (*usb_ep_handler_ptr)(void);
/* EP0 command  interrupt handler */
typedef void (*usb_ep0_cmd_ptr)(Usb_Command *pcmd);

typedef void (*usb_void_func)(void);

/* endpoint information, including endpoint status */
typedef struct 
{
	union 
	{
		Usb_Ep_Dscr			stdep;			/* ep descriptor */
		uint8_t			classep[USB_MAX_CLASS_EPDESC_LENGTH];
	}epdesc;

	union 
	{
		Usb_EpBIn_Status	epin_status;
		Usb_EpBOut_Status	epout_status;
	}ep_status;						/* ep status */

	usb_ep_handler_ptr		ep_reset;		/* ep reset handler */
	uint16_t				epdscr_size;		/* descriptor length */
}Usb_Ep_Info;


typedef struct 
{
	Usb_Ep_Info			*ep_info[USB_MAX_EP_PER_IF];/* endpoint attach to the interface info */	
	uint16_t			ifdscr_size;		/* descriptor length */
	Usb_If_Dscr			stdif;	
}Usb_Alternate_Interface_Info;


/* interface information, including endpoint information and interface information*/
typedef struct 
{
	char			*interface_name_ptr;	/* interface name */
	usb_class_specific_handler_ptr	if_class_specific_hdlr;	/* interface specific handler, handle ep0 class specific request */
	uint16_t			ifdscr_size;		/* descriptor length */
	union 
	{
		Usb_If_Dscr		stdif;
		uint8_t		classif[USB_MAX_IFDSC_LENGTH];
	}ifdscr;
	
	Usb_Ep_Info			*ep_info[USB_MAX_EP_PER_IF];/* endpoint attach to the interface info */	
	
#if  defined(__USB_SUPPORT_ISO_PIPE__)||defined(__MBIM_SUPPORT__)||defined(__ECM_SUPPORT__)
	Usb_Alternate_Interface_Info	alternate_if_info[USB_MAX_INTERFACE_ALTERNATE_NUM];
	uint8_t				alternate_setting;	/* alternate setting number */
#endif

}Usb_Interface_Info;


/* configuration information */
typedef struct 
{	
	Usb_Cfg_Dscr		stdcfg;			/* config descriptor */
}Usb_Config_Info;


typedef struct
{
	bool			b_enable;
	uint8_t			cmd;
	usb_ep0_cmd_ptr	ep0_cmd_hdlr;
}Usb_EP0_Cmd_Hdler;

/* device information, used for usb level */
typedef struct
{
	USB_DEVICE_TYPE 	device_type;
	USB_DEVICE_STATE	nDevState;			/*DEVSTATE_CONFIG,DEVSTATE_ADDRESS,DEVSTATE_DEFAULT*/		
	bool			remoteWk;

	uint32_t  			memory_addr;// allocate memory buffer address
	uint32_t  			reserved_memory_size;//memory left size	

	bool			self_powered;
	bool			ep0_send_one_pkt;		/* the variable to record ep0 will send only one packet */
	bool			usb_send_config_result; /* to send configure may be fail */
	
	uint8_t			config_num;
	uint8_t			interface_num[USB_MAX_INTERFACE];
	USB_EP0_STATE 		ep0_state;


	/* used for resource management*/
// optimize code size : cannot change order
   	uint8_t			resource_ep_bulk_tx_number;
   	uint8_t			resource_ep_bulk_rx_number;		
   	uint8_t			resource_ep_intr_tx_number;
	uint8_t			resource_interface_number;
   	uint8_t			resource_string_number;   
	uint8_t			resource_iad_number;   		
// resource_iad_number : must be the end of resource parameter



	/*  the configuration only one for saving memory  */
// optimize code size : cannot change order
	Usb_Dev_Dscr		devdscr;
	Usb_Cfg_Dscr 		cfgdscr;
	Usb_Ep_Info			ep_bulk_tx_info[USB_MAX_EP_BULK_TX];
	Usb_Ep_Info			ep_bulk_rx_info[USB_MAX_EP_BULK_RX];
	Usb_Ep_Info			ep_intr_tx_info[USB_MAX_EP_INTR_TX];
	Usb_Interface_Info	if_info[USB_MAX_INTERFACE];	
	Usb_String_Dscr		*resource_string[USB_MAX_STRING];
	Usb_IAD_Dscr			iad_desc[USB_MAX_IAD];
	
	Usb_Dev_Qual_Dscr			dev_qual_dscr;
	Usb_Other_Speed_Cfg_Dscr		other_speed_cfg_dscr;
	hal_usb_test_mode_type_t			usb_test_type;
	uint8_t 			*conf;				/*just for descriptor transfer*/
// conf :  : must be the end of descriptor parameter

	Usb_Command			cmd;				/*USB_COMMAND*/

	Usb_Ep0_Status    	ep0info;
	usb_ep0_rx_ptr  	ep0_rx_handler;
	Usb_EP0_Cmd_Hdler	ep0_class_cmd_handler; /* class specific ep0 cmd */

   	/* serial string unique for each phone*/
   	uint16_t			serial_string[USB_SERIAL_STRING_LEN]; 

	/* customizable variables*/
	const USB_DEVICE_PARAM	*device_param;
} Usb_Device;

#ifdef  __USB_MASS_STORAGE_ENABLE__
/* event for callback */
typedef enum {
    MSC_EVENT_USB_CONNECTION,
    MSC_EVENT_USB_DISCONNECTION,
} msc_callback_event_t;

typedef void (*msc_register_callback_t)(msc_callback_event_t event);
#endif

typedef enum {
    /* USB_COMMON */
    USB_CABLE_MSG                 = 1,
    /* USB_ACM */
    USB_ACM_MSG                 = 2,
    /* USB_MSC */
	USB_MSC_TX_MSG 				= 3,
	USB_MSC_RX_MSG              = 4,
	USB_MSC_CLR_STALL_MSG		= 5,
	USB_MSC_RESET_IND_MSG		= 6,	
} usb_msg_type_t;

typedef struct {
    usb_msg_type_t msg_id;
    void *data;
} usb_msg_t;


/***********************************************
	function and global variable
************************************************/
extern Usb_Device gUsbDevice;
void USB_EP0_Command_Hdlr(bool bError);

extern USB_DEVICE_STATE USB_Get_Device_State(void);
extern USB_DEVICE_TYPE USB_Get_Device_Type(void);
extern void USB_Init(USB_DEVICE_TYPE type);

extern void USB_Release_Type(void);
extern void USB_Free_Memory(void);
extern void RestoreIRQMask(uint32_t x);
extern void USB_Generate_EP0Data(Usb_Ep0_Status*pep0state, Usb_Command*pcmd, void *data, int32_t len);
extern uint32_t SaveAndSetIRQMask( void );
extern unsigned int USB_Get_Memory(unsigned int len);
extern unsigned int USB_Bulk_Max_Packet_Size(void);
extern unsigned int USB_Intr_Max_Packet_Size(void);
extern unsigned int USB_Speed_Reset_Packet_Size(bool b_other_speed);
extern void usb_boot_init(void);
bool ap_usb_init(USB_DEVICE_TYPE type);
bool ap_usb_deinit(void);
bool is_vusb_ready(void);

bool usb_task_init(void);
void USB_Send_Message(usb_msg_type_t msg, void *data);


#ifdef  __USB_MASS_STORAGE_ENABLE__
void ap_usb_register_msc_callback(msc_register_callback_t callback);
#endif

#endif /* USB_H */

