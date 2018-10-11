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

#ifndef USBACM_DRV_H
#define USBACM_DRV_H

#include "usb.h"
#include "usb_comm.h"
#include "usb_custom.h"
#include "bmd.h"

#ifdef MTK_PORT_SERVICE_ENABLE
#include "serial_port.h"
#endif

/***********************************************
	acm class specific command definition
************************************************/

#define USB_ACM_DEVICE_CODE			0x02
#define USB_ACM_SUBCLASS_CODE			0x00
#define USB_ACM_PROTOCOL_CODE			0x00

#define USB_ACM_COMM_INTERFACE_CLASS_CODE 	0x02
#define USB_ACM_COMM_INTERFACE_SUBCLASS_CODE 	0x02
#define USB_ACM_COMM_INTERFACE_PROTOCOL_CODE 	0x01

#define USB_ACM_DATA_INTERFACE_CLASS_CODE 	0x0a
#define USB_ACM_DATA_INTERFACE_SUBCLASS_CODE 	0x00
#define USB_ACM_DATA_INTERFACE_PROTOCOL_CODE 	0x00

#define USB_CDC_IF_LENGTH     			28   /* standard plus class */

/* Class-Specfic Codes ,EP0*/
#define USB_ACM_SEND_ENCAPSULATED_COMMAND	0x00
#define USB_ACM_GET_ENCAPSULATED_RESPONSE	0x01
#define USB_ACM_SET_COMM_FEATURE		0x02
#define USB_ACM_GET_COMM_FEATURE		0x03
#define USB_ACM_CLEAR_COMM_FEATURE		0x04
#define USB_ACM_SET_LINE_CODING			0x20
#define USB_ACM_GET_LINE_CODING			0x21
#define USB_ACM_SET_CONTROL_LINE_STATE		0x22
#define USB_ACM_SEND_BREAK			0x23

/* Class-Specfic Codes ,EP0 - NCM*/
#define USB_NCM_SEND_ENCAPSULATED_COMMAND	0x00
#define USB_NCM_GET_ENCAPSULATED_RESPONSE	0x01
#define USB_NCM_GET_NTB_PARAMETERS			0x80
#define USB_NCM_GET_NET_ADDRESS			0x81
#define USB_NCM_SET_NET_ADDRESS			0x82
#define USB_NCM_GET_NTB_INPUT_SIZE			0x85
#define USB_NCM_SET_NTB_INPUT_SIZE			0x86
#define USB_NCM_RESET						0x05


/*Notifications, interrupt pipe*/
#define USB_ACM_NOTIF_NETWORK_CONNECTION	0x00
#define USB_ACM_NOTIF_RESPONSE_AVAILABLE	0x01
#define USB_ACM_NOTIF_SERIAL_STATE		0x20

/* UART state bitmap values */
#define SERIAL_STATE_BRXCARRIER			0x01


/*USB_ACM_SET_CONTROL_LINE_STATE*/    /*  not use anywhere  */
#define CONTROL_LINE_SIGNAL_RTS			0x02  /*DCE's RTS,DTE's CTS*/
#define CONTROL_LINE_SIGNAL_DTR			0x01  /*DCE's DTR,DTE's DSR*/


// RNDIS Spec  //
#define USB_ISD_DEVICE_CODE			0xE0 //0x02	//0xE0
#define USB_ISD_SUBCLASS_CODE		0x01 //0x00	//0x01
#define USB_ISD_PROTOCOL_CODE		0x03 //0x00	//0x03

#define USB_ISD_COMM_INTERFACE_CLASS_CODE 		0xE0 //0x02 	//0xE0
#define USB_ISD_COMM_INTERFACE_SUBCLASS_CODE 	0x01 //0x02 	//0x01
#define USB_ISD_COMM_INTERFACE_PROTOCOL_CODE 	0x03 //0xff 	//0x03

#define USB_ISD_DATA_INTERFACE_CLASS_CODE 	0x0A
#define USB_ISD_DATA_INTERFACE_SUBCLASS_CODE 	0x00
#define USB_ISD_DATA_INTERFACE_PROTOCOL_CODE 	0x00

#define USB_RNDIS_RESPONSE_AVAILABL			0x01

#define RNDIS_RX_BUFFER_NUM  				USB_ACM_RX_BUFFER_NUM   // UPS : put 8 buffer
#if defined(__MBIM_SUPPORT__)||defined(__ECM_SUPPORT__)
#define RNDIS_RX_FLC_BUFFER_NUM  			16 // UPS : use 16 buffer in MBIM
#else
#define RNDIS_RX_FLC_BUFFER_NUM  			3 // UPS : use 3 buffer 
#endif
#define RNDIS_MAX_FRAME			1560

#define USB_RNDIS_RX_LOOP_COUNT  			16



//MBIM
#define USB_MBIM_IF_LENGTH     			(31+8)   /* standard plus class */

#define USB_MBIM_DEVICE_CODE		0xEF
#define USB_MBIM_SUBCLASS_CODE		0x02
#define USB_MBIM_PROTOCOL_CODE		0x01
#define USB_MBIM_DESCRIPTOR_CODE	0x1B	//MBIM functional descriptor
#define USB_MBIM_EXT_DESCRIPTOR_CODE	0x1C	//MBIM extended functional descriptor

#define USB_MBIM_COMM_INTERFACE_CLASS_CODE		0x02
#define USB_MBIM_COMM_INTERFACE_SUBCLASS_CODE	0x0E
#define USB_MBIM_COMM_INTERFACE_PROTOCOL_CODE	0x00

#define USB_MBIM_DATA_INTERFACE_CLASS_CODE		0x0A
#define USB_MBIM_DATA_INTERFACE_SUBCLASS_CODE	0x00
#define USB_MBIM_DATA_INTERFACE_PROTOCOL_CODE	0x02

// ECM Spec  //
#define USB_ECM_IF_LENGTH  32
#define USB_ECM_DEVICE_CODE			0x02 
#define USB_ECM_SUBCLASS_CODE		0x06 
#define USB_ECM_PROTOCOL_CODE		0x00 

#define USB_ECM_COMM_INTERFACE_CLASS_CODE 		0x02 
#define USB_ECM_COMM_INTERFACE_SUBCLASS_CODE 	0x06 
#define USB_ECM_COMM_INTERFACE_PROTOCOL_CODE 	0x00 

#define USB_ECM_DATA_INTERFACE_CLASS_CODE 		0x0A
#define USB_ECM_DATA_INTERFACE_SUBCLASS_CODE 	0x00
#define USB_ECM_DATA_INTERFACE_PROTOCOL_CODE 	0x00

#define USB_ECM_SET_ETHERNET_MULTICAST_FILTERS					0x40
#define USB_ECM_SET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER	0x41
#define USB_ECM_GET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER	0x42
#define USB_ECM_SET_ETHERNET_PACKET_FILTER						0x43
#define USB_ECM_GET_ETHERNET_STATIC								0x44


#define ECM_NETWORK_CONNECTION  		0x00 	
#define ECM_RESPONSE_AVAILABLE  		0x01 	   
#define ECM_CONNECYION_SPEED_CHANGE  	0x2A  

/***********************************************
	Implement definition
************************************************/

#define USB_CDC_ACM_IF_NUMBER      				2

#define USB_CDC_ACM_COMM_EP_NUMBER     		1  /*interrupt pipe*/
#define USB_CDC_ACM_DATA_EP_NUMBER     		2

#define USBACM_RING_BUFFER_TIMEOUT			25

#define USBACM_HIGH_SPEED_LOOP				100
#define USBACM_DIRECT_PATH_LOOP				500


#define USB_ACM_RX_HDLR_PACKET_NUM  			16


/***********************************************
	UART SW Implement definition
************************************************/
#define  UART_STAT_EscDet        0x01
#define  UART_STAT_Break         0x02

#define  UART_RecNormal          0
#define  UART_Get3EscChar        1
#define  UART_StartCheckESC      2

typedef void (*UART_TX_FUNC)(uint32_t port); 
typedef void (*UART_RX_FUNC)(uint32_t port) ; 

/***********************************************
	acm class specific descriptor structure
************************************************/

/* CDC ACM communication interface descriptor element */
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

	uint8_t    HFbFunctionLength;		/*Header Functional Descriptor*/
	uint8_t    HFbDescriptorType;
	uint8_t    HFbDescriptorSubtype;
	uint16_t   bcdCDC;

	uint8_t    ACMFbFunctionLength;	/*Abstract Control Management Functional Descriptor*/
	uint8_t    ACMFbDescriptorType;
	uint8_t    ACMFbDescriptorSubtype;
	uint8_t    ACMFbmCapabilities;

	uint8_t    UFbFunctionLength;		/*Union Functional Descriptor*/
	uint8_t    UFbDescriptorType;
	uint8_t    UFbDescriptorSubtype;
	uint8_t    bMasterInterface;
	uint8_t    bSlaveInterface0;

	uint8_t    CMFbFunctionLength;	/*Call Management Descriptor*/
	uint8_t    CMFbDescriptorType;
	uint8_t    CMFbDescriptorSubtype;
	uint8_t    CMFbmCapabilities;
	uint8_t    bDataInterface;
} Usb_Cdc_If_Dscr;


//#if defined(__ARMCC_VERSION)
//typedef struct __packed
//#elif defined(__GNUC__)
//typedef struct __attribute__ ((__packed__))
//#else
typedef struct
//#endif
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

	uint8_t    HFbFunctionLength;		/*Header Functional Descriptor*/
	uint8_t    HFbDescriptorType;
	uint8_t    HFbDescriptorSubtype;
	uint16_t   bcdCDC;

	uint8_t    UFbFunctionLength;		/*Union Functional Descriptor*/
	uint8_t    UFbDescriptorType;
	uint8_t    UFbDescriptorSubtype;
	uint8_t    bMasterInterface;
	uint8_t    bSlaveInterface0;
	
	uint8_t    MBIMbFunctionLength;	/*MBIM Functional Descriptor*/
	uint8_t    MBIMbDescriptorType;
	uint8_t    MBIMbDescriptorSubtype;
	uint16_t   MBIMbcdMBIMVersion;
	uint16_t   MBIMwMaxControlMessage;
	uint8_t    MBIMbNumberFiliters;
	uint8_t    MBIMbMaxFiliterSize;
	uint16_t   MBIMwMaxSegmentSize;
	uint8_t    MBIMbmNetworkCapabilities;
    uint8_t    MBIMEXTbFuntionLength; /*MBIM Extended Functional Descriptor*/
    uint8_t    MBIMEXTbDescriptorType;
    uint8_t    MBIMEXTbDescriptorSubtype;
    uint16_t   MBIMEXTbcdMBIMExtendedVersion;
    uint8_t    MBIMEXTbMaxOutstandingCommandMessages;
    uint16_t   MBIMEXTwMTU;
} Usb_MBIM_If_Dscr;


/* CDC ECM communication interface descriptor element */
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

	uint8_t    HFbFunctionLength;		/* Header Functional Descriptor */
	uint8_t    HFbDescriptorType;
	uint8_t    HFbDescriptorSubtype;
	uint16_t   bcdCDC;

	uint8_t    UFbFunctionLength;		/* Union Functional Descriptor */
	uint8_t    UFbDescriptorType;
	uint8_t    UFbDescriptorSubtype;
	uint8_t    bMasterInterface;
	uint8_t    bSlaveInterface0;	

	uint8_t    ENFbFunctionLength;    /* Ethernet Networking Functional Descriptor */
	uint8_t	 ENFbDescriptorType;
	uint8_t 	 ENFbDescriptorSubtype;
	uint8_t    iMACAddress;
	uint8_t   bmEthernetStatistics1;
	uint8_t   bmEthernetStatistics2;	
	uint8_t   bmEthernetStatistics3;
	uint8_t   bmEthernetStatistics4;	
	uint8_t   wMaxSegmentSize1;
	uint8_t   wMaxSegmentSize2;	
	uint8_t   wNumberMCFilters1;
	uint8_t   wNumberMCFilters2;	
	uint8_t    bNumberPowerFilters;

} Usb_Ecm_If_Dscr;


/*  ACM line coding element */
typedef struct
{
	uint32_t  dwDTERate;
	uint8_t   bCharFormat;
	uint8_t   bParityType;
	uint8_t   bDataBits;
} UsbAcm_Line_Coding;

typedef uint32_t UART_BAUDRATE_T;
typedef UART_BAUDRATE_T UART_baudrate;
typedef uint16_t   UART_PORT;

#define     UART_BAUD_AUTO          0
#define     UART_BAUD_75            75
#define     UART_BAUD_110           110
#define     UART_BAUD_150           150
#define     UART_BAUD_300           300
#define     UART_BAUD_600           600
#define     UART_BAUD_1200          1200
#define     UART_BAUD_2400          2400    
#define     UART_BAUD_4800          4800
#define     UART_BAUD_7200          7200
#define     UART_BAUD_9600          9600
#define     UART_BAUD_14400         14400
#define     UART_BAUD_19200         19200
#define     UART_BAUD_28800         28800
#define     UART_BAUD_33900         33900
#define     UART_BAUD_38400         38400
#define     UART_BAUD_57600         57600
#define     UART_BAUD_115200        115200
#define     UART_BAUD_230400        230400
#define     UART_BAUD_460800        460800
#define     UART_BAUD_921600        921600
#define     UART_BAUD_1500000       1500000 

typedef enum {
      len_5=5,
      len_6,
      len_7,
      len_8
} UART_bitsPerCharacter;

typedef enum {
      sb_1=1,
      sb_2,
      sb_1_5
} UART_stopBits;

typedef enum {
      pa_none=0,
      pa_odd,
      pa_even,
      pa_mark,
      pa_space
} UART_parity;

typedef enum {
      fc_none=1,
      fc_hw,
      fc_sw
} UART_flowCtrlMode;

typedef struct
{
      UART_baudrate                 baud; 
      UART_bitsPerCharacter         dataBits;
      UART_stopBits                 stopBits;
      UART_parity                   parity;
      UART_flowCtrlMode             flowControl;
      uint8_t                     xonChar;
      uint8_t                     xoffChar;
      bool                      DSRCheck;
} UARTDCBStruct;


/* ACM interrupt packet structure */
typedef struct
{
	uint8_t		bmRequestType;
	uint8_t		bRequest;
	uint16_t	wValue;
	uint16_t	wIndex;
	uint16_t	wLength;
	uint16_t	Data;
}UsbAcm_Intr_Pkt;


/* ECM interrupt packet structure */
typedef struct
{
	uint8_t		  bmRequestType;
	uint8_t		  bNotificationCode;
	uint16_t	  wValue;
	uint16_t	  wIndex;
	uint16_t	  wLength;
	uint8_t	    Data[8];
}UsbEcm_Intr_Pkt;

/***********************************************
	implement enum and structure 
************************************************/

/* transmit buffer type */
typedef enum 
{
	USBTRX_MEM_UNKOWN = 0,
	USBTRX_MEM_ISR,
	USBTRX_MEM_TASK,
	USBTRX_MEM_MAX_TYPE
} USBTRX_MEM_TYPE;

typedef enum 
{
	ACM_INTR_IDLE = 0,
	ACM_INTR_SEND_ONE_PKT,
	ACM_INTR_SEND_LAST_PKT
}USB_ACM_INTR_PIPE_STATE;


/* Current ACM driver owner application */
typedef enum
{
	USB_ACM_OWNER_UART = 0,
	USB_ACM_OWNER_FT
//	USB_ACM_OWNER_LOGGING
}USB_ACM_OWNER_TYPE;



typedef enum 
{
	CDC_NORMAL = 0,  	    //  old transfer type, save data in ring buffer, and copy data to buffer by single interrupt    ex:USB2UART_GetBytes
	CDC_HIGH_SPEED,   	//  Only get data   ( there might be no FLC buffer anymore)
	CDC_HIGH_SPEED_FRAME,   	    //  use transfer as unit
	CDC_HIGH_SPEED_FRAME_DMAQ   	    //  use transfer as unit with DMAQ
}USB_Transfer_type;

/**
 * TGPD
 */
typedef struct _TGPD
{
	uint8_t flag;
    uint8_t  chksum;
    uint16_t  DataBufferLen; /*Rx Allow Length*/
    struct _TGPD* pNext;
    uint8_t* pBuf;
    uint16_t bufLen;
    uint8_t ExtLength;
    uint8_t ZTepFlag;
}TGPD, *PGPD;

#define RXGPD_FLAGS_HWO           0x01
#define RXGPD_FLAGS_BDP       		 0x02
#define RXGPD_ENDPOINT          	0x0F
#define RXGPD_TGL          			0x10
#define RXGPD_ZLP          			0x20
#define RXGPD_FLAGS_IOC           0x80


#define TXGPD_FLAGS_HWO              0x01
#define TXGPD_FORMAT_BDP             0x02
#define TXGPD_FORMAT_TGL             0x10
#define TXGPD_FORMAT_ZLP             0x20
#define TXGPD_FLAG_IOC				0x40


typedef enum 
{
	CDC_MBIM_CLASS = 1,   	   
	CDC_ECM_CLASS,   
}USB_CDC_type;


typedef void (*usb_acm_rx_ptr)(void);

/* ACM device structure */
typedef struct
{
	uint8_t		data_interface_id;	
	bool 		send_Txilm;
	bool 		send_Rxilm;
	bool		send_UARTilm;
	bool		config_send_Txilm;
	bool		ring_buffer_timer_counting;		/* flag to check ring buffer timer running or not */
	bool		threshold_enable;				/* enable ring buffer threshold and time out mechanism */
	bool		setup_dma;						/* a flag to record DMA is triggered or not */
	bool		is_intr_pending_pkt;				/* interrupt pipe has pending packet */
	bool 		dma_txcb_just_done;			/*DMA TX Callback Function just done. It's clear by putbyte funtion*/
	bool 		break_detect;
	bool		put_start;
	uint16_t      dmaf_setmember;	  /* Who set dma_false after putbyte and it does not finsih*/
	uint16_t		line_state;
	UsbAcm_Intr_Pkt		intr_pkt;	
#ifdef __ECM_SUPPORT__	
	UsbEcm_Intr_Pkt     notify_pkt;
	uint16_t			power_filter_number;
	uint16_t			power_filter_length;
	uint16_t			*mac_addr_ptr;
#endif
	uint32_t		handle;    		/*GPT handle*/
	uint32_t		dma_handle;    		/*for DMA timeout handle*/
	uint32_t		ring_buffer_handle;	/* ring buffer's GPT handle */
	uint32_t 		break_number;
	UsbAcm_Line_Coding	line_coding;		/* current line coding, just used to reply to host */	

	Usb_EpBIn_Status	*txpipe;		/* bulk in EP, Data interface */
	Usb_EpBOut_Status	*rxpipe;		/* bulk out EP, Data interface */
	Usb_EpBIn_Status	*intrpipe;		/* interrupt EP, Communication Interface */
	
	USB_ACM_INTR_PIPE_STATE	intr_state;				/* interrupt pipe state machine */
	Usb_Ep_Info			*comm_ep_intr_info;
	Usb_Ep_Info			*data_ep_in_info;
	Usb_Ep_Info			*data_ep_out_info;
	Usb_Interface_Info	*comm_if_info;
	Usb_Interface_Info	*data_if_info;
#ifdef  __MBIM_SUPPORT__
	Usb_Alternate_Interface_Info *mbim_if_alter_info;
#endif
#ifdef  __ECM_SUPPORT__
	Usb_Alternate_Interface_Info *ecm_if_alter_info;
#endif

	USB_Transfer_type	transfer_type;
	
//#if defined(__USB_COMPOSITE_DEVICE_SUPPORT__)
		Usb_IAD_Dscr		*iad_desc;
//#endif
	
#ifdef  __USB_HIGH_SPEED_COM_PORT_ENABLE__
	uint32_t	data_addr;
	uint32_t	rx_buf_data_len[USB_ACM_RX_BUFFER_NUM];
	uint16_t	tx_count;  		/* To store the TX count, debug only*/
	bool		b_data_left; // left data due to 512N
	bool		b_is_high_speed_enable;
	bool		b_rc_index_move;
	uint16_t		b_rc_get_count;	
	uint8_t		rx_buf_rc_index;  /* read clear pointer */
	uint8_t		rx_buf_r_index;  /* read pointer */
	uint8_t		rx_buf_w_index;  /* write pointer */
	uint8_t		rx_buf_count;  /* the buffer count with data */
#ifdef DRV_USB_DMA_QUEUE_SUPPORT
	bool	ppp_txdmaq_first;
	uint32_t	ppp_rdmaq_idx;
	uint32_t	ppp_tdmaq_idx;
#endif
//#endif

//#ifdef __USB_TETHERING__
	uint8_t 		*rx_buf_addr[RNDIS_RX_BUFFER_NUM+1];	//9 : 0 ~ 8
	uint32_t		isd_rx_buf_data_len[RNDIS_RX_BUFFER_NUM+1];
	uint8_t 		*rx_flc_buf_addr[RNDIS_RX_FLC_BUFFER_NUM+1];	 //3 //3:  0~2
	uint32_t		rx_flc_buf_data_len[RNDIS_RX_FLC_BUFFER_NUM+1];
	uint16_t		tx_flc_count;  		/* To store the TX count, debug only*/
	uint8_t		*tx_flc_data_addr;
	bool		rx_full_data;
	bool		halt_flag;
	bool		ctrl_data_done;
	bool		send_ctrl_Txilm;  // Ctrl pipe --> TX message
	bool		send_ctrl_Rxilm;  // ctrl pipe --> RX message
	
//	bool		dma_rx_running;  //use  FIFO Read first 
//	bool		dma_tx_running; //the dame as setup_dma

	uint8_t		rx_full_buff_index;  // receive Data 
	uint8_t		rx_empty_buff_index; // empty buffer
	uint8_t		rx_flc_full_buff_index;  // receive Data 
	uint8_t		rx_flc_empty_buff_index; // empty buffer 
//	bool		b_flc_rc_index_move;
	uint8_t		rx_flc_buf_rc_index;  /* read clear pointer */
	uint8_t		rx_flc_buf_r_index;  /* read pointer */
	uint8_t		rx_flc_buf_w_index;  /* write pointer: data write (bulk out handler) */
	uint8_t		rx_flc_buf_count;  /* the buffer count with data */
	uint8_t		rx_flc_temp_length;  /* for first time PC send data just before buffer push  (initial message) */
	bool		rx_flc_temp;  /* for first time PC send data just before buffer push (initial message) */	
	uint8_t		*rx_flc_temp_buf_addr;
#endif

	bool		b_is_tx_direct_path_enable;

}UsbAcm_Struct;



typedef struct
{
	/* FT mode param */
	USB_ACM_OWNER_TYPE	acm_owner;

	/* Customizable variables */
	const USB_ACM_PARAM	*acm_param;
}UsbAcm_Common_Struct;

typedef struct
{
      uint8_t                      *rx_buffer;   /* data buffer */
      uint8_t                      *tx_buffer;   /* data buffer */
} UART_RingBufferStruct;

typedef struct
{
      uint32_t                      port_no;
      bool                       initialized;
      bool                       power_on;
      //bool                       breakDet;
      //bool                       EscFound;
      //IO_level                       DSR;  

      uint8_t                      EscCount;
      uint8_t                      Rec_state; /**/

   /*detect Escape*/
      uint32_t                      handle;    /*GPT handle*/
   
      UARTDCBStruct				  DCB;
      UART_RingBufferStruct		      RingBuffers;		   
      BUFFER_INFO                    Rx_Buffer;  /* receive buffer */
      BUFFER_INFO                    Tx_Buffer;  /* transmit buffer */
      //BUFFER_INFO                    Tx_Buffer_ISR; /* receive buffer */

      UART_TX_FUNC                  tx_cb;
      UART_RX_FUNC                  rx_cb;

} UsbUARTStruct;


/***********************************************
	function and global variable
************************************************/
extern UsbUARTStruct USB2UARTPort[MAX_USB_PORT_NUM];
extern UsbAcm_Struct g_UsbACM[MAX_USB_PORT_NUM];
extern UsbAcm_Common_Struct g_UsbACM_Comm;

#ifdef MTK_PORT_SERVICE_ENABLE
extern uint8_t USB_PORT[SERIAL_PORT_DEV_MAX];
#else
extern uint8_t USB_PORT[2];
#endif

extern void USB_Acm_Rx_ClrFifo(uint8_t port);
extern void USB_Acm_CommIf_Create(uint8_t port, void *ifname);
extern void USB_Acm_CommIf_Reset(uint8_t port);
extern void USB_Acm_CommIf_Enable(uint8_t port);
extern void USB_Acm_CommIf_Speed_Reset(uint8_t port, bool b_other_speed);
extern void USB_Acm_DataIf_Create(uint8_t port,void *ifname);
extern void USB_Acm_DataIf_Reset(uint8_t port);
extern void USB_Acm_DataIf_Enable(uint8_t port);
extern void USB_Acm_DataIf_Speed_Reset(uint8_t port, bool b_other_speed);
extern void USB_Init_Acm_Status(void);
extern void USB_Release_Acm_Status(void);
extern void USB_UnMask_COM_Intr(uint32_t port);


extern void USB_Acm1_CommIf_Create(void *ifname);
extern void USB_Acm1_CommIf_Reset(void);
extern void USB_Acm1_CommIf_Enable(void);
extern void USB_Acm1_CommIf_Speed_Reset(bool b_other_speed);
extern void USB_Acm1_DataIf_Create(void *ifname);
extern void USB_Acm1_DataIf_Reset(void);
extern void USB_Acm1_DataIf_Enable(void);
extern void USB_Acm1_DataIf_Speed_Reset(bool b_other_speed);

extern void USB_Acm2_CommIf_Create(void *ifname);
extern void USB_Acm2_CommIf_Reset(void);
extern void USB_Acm2_CommIf_Enable(void);
extern void USB_Acm2_CommIf_Speed_Reset(bool b_other_speed);
extern void USB_Acm2_DataIf_Create(void *ifname);
extern void USB_Acm2_DataIf_Reset(void);
extern void USB_Acm2_DataIf_Enable(void);
extern void USB_Acm2_DataIf_Speed_Reset(bool b_other_speed);

extern void USB2UART_Tx_DMA1_Callback(void);
extern void USB2UART_Tx_DMA2_Callback(void);
extern void USB2UART_Dafault_Tx_Callback(uint32_t port);
extern void USB2UART_Dafault_Rx_Callback(uint32_t port);
extern void USB2UART_DMATransmit(uint32_t port);


#endif /* USBACM_DRV_H */

