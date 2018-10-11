/*
** $Id: //Department/DaVinci/TRUNK/MT6620_WiFi_Firmware/mcu/wifi/include/typedef.h#6 $
*/

/*! \file   typedef.h
    \brief  Declaration of data type and return values of internal protocol stack.

    In this file we declare the data type and return values which will be exported
    to the GLUE Layer.
*/

/*******************************************************************************
* Copyright (c) 2007 MediaTek Inc.
*
* All rights reserved. Copying, compilation, modification, distribution
* or any other use whatsoever of this material is strictly prohibited
* except in accordance with a Software License Agreement with
* MediaTek Inc.
********************************************************************************
*/

/*******************************************************************************
* LEGAL DISCLAIMER
*
* BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND
* AGREES THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK
* SOFTWARE") RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE
* PROVIDED TO BUYER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY
* DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT
* LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE
* ANY WARRANTY WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY
* WHICH MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK
* SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY
* WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE
* FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION OR TO
* CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
* BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
* LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL
* BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT
* ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY
* BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
* WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT
* OF LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING
* THEREOF AND RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN
* FRANCISCO, CA, UNDER THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE
* (ICC).
********************************************************************************
*/

/*
** $Log: typedef.h $
 *
 * 05 21 2015 raghav.monga
 * [BORA00004254] [MT7637E2][IOT][FW]
 * Remove RAM header file dependancy in ROM code.
 * Created parallel header files for RAM files which were being used in ROM code.
**
** 02 14 2015 ted.huang
** [BORA00003379] [MT7637]
** Merge 7615 FW RAM change .
**
** 09 15 2014 ted.huang
** [BORA00002807] [MT7636]
** merge MT7636 back main trunk.
**
** 07 15 2014 tiger.hung
** [BORA00003241] [MT7636] sync MT7636 E1 MP source code
**
*/

#ifndef _TYPEDEF_H
#define _TYPEDEF_H


/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/


#if 0
#define WLAN_STATUS_SUCCESS                     ((WLAN_STATUS)0x00000000L)
#define WLAN_STATUS_FAILURE                     ((WLAN_STATUS)0xC0000001L)

#define WLAN_STATUS_PENDING                     ((WLAN_STATUS)0x00000103L)
#define WLAN_STATUS_NOT_ACCEPTED                ((WLAN_STATUS)0x00010003L)

#define WLAN_STATUS_MEDIA_CONNECT               ((WLAN_STATUS)0x4001000BL)
#define WLAN_STATUS_MEDIA_DISCONNECT            ((WLAN_STATUS)0x4001000CL)
#define WLAN_STATUS_MEDIA_SPECIFIC_INDICATION   ((WLAN_STATUS)0x40010012L)

#define WLAN_STATUS_SCAN_COMPLETE               ((WLAN_STATUS)0x60010001L)
#define WLAN_STATUS_MSDU_OK                     ((WLAN_STATUS)0x60010002L)

#define WLAN_STATUS_ROAM_OUT_FIND_BEST          ((WLAN_STATUS)0x60010101L)
#define WLAN_STATUS_ROAM_DISCOVERY              ((WLAN_STATUS)0x60010102L)

#define WLAN_STATUS_RESOURCES                   ((WLAN_STATUS)0xC000009AL)
#define WLAN_STATUS_NOT_SUPPORTED               ((WLAN_STATUS)0xC00000BBL)

#define WLAN_STATUS_MULTICAST_FULL              ((WLAN_STATUS)0xC0010009L)
#define WLAN_STATUS_INVALID_PACKET              ((WLAN_STATUS)0xC001000FL)
#define WLAN_STATUS_ADAPTER_NOT_READY           ((WLAN_STATUS)0xC0010011L)
#define WLAN_STATUS_INVALID_LENGTH              ((WLAN_STATUS)0xC0010014L)
#define WLAN_STATUS_INVALID_DATA                ((WLAN_STATUS)0xC0010015L)
#define WLAN_STATUS_BUFFER_TOO_SHORT            ((WLAN_STATUS)0xC0010016L)

#define WLAN_STATUS_BUFFER_RETAINED             ((WLAN_STATUS)0x00000001L)
#define WLAN_STATUS_PACKET_FLUSHED              ((WLAN_STATUS)0x00000002L)
#define WLAN_STATUS_PACKET_AGING_TIMEOUT        ((WLAN_STATUS)0x00000003L)
#define WLAN_STATUS_PACKET_MPDU_ERROR           ((WLAN_STATUS)0x00000004L)
#define WLAN_STATUS_PACKET_RTS_ERROR            ((WLAN_STATUS)0x00000005L)
#define WLAN_STATUS_PACKET_LIFETIME_ERROR       ((WLAN_STATUS)0x00000006L)

#endif

/*
 * NIC status flags
 */
#define ADAPTER_FLAG_HW_ERR                     0x00400000

/*
 * Type Length
 */
#define TL_IPV4                                 0x0008
#define TL_IPV6                                 0xDD86


/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
/*
 * Type definition for WLAN STATUS
 */
typedef UINT_32 WLAN_STATUS, *P_WLAN_STATUS;

/*
 * Type definition for STA_RECORD_T structure to handle the connectivity and packet reception for a particular STA
 */
typedef struct _STA_RECORD_T STA_RECORD_T, *P_STA_RECORD_T;

/*
 * Type definition for MSDU_INFO structure, a SW resource for outgoing packet
 */
typedef struct _MSDU_INFO_T MSDU_INFO_T, *P_MSDU_INFO_T, **PP_MSDU_INFO_T;

/*
 * Type definition for SW_RFB_T structure, a SW resource for incoming packet
 */
typedef struct _SW_RFB_T SW_RFB_T, *P_SW_RFB_T, **PP_SW_RFB_T;

/*
 * Type definition for TX_PKT_INFO_T structure, a SW resource for outgoing packet
 */
typedef struct _TX_PKT_INFO_T TX_PKT_INFO_T, *P_TX_PKT_INFO_T, **PP_TX_PKT_INFO_T;

/*
 * Type definition for BSS_INFO_T structure, to describe the attributes used in
 * a common BSS. NOTE: Before we use struct _P2P_DEV_INFO_T to save DLM size,
 * each SW module code shall be reviewed carefully. 2013/8/30
 */
typedef struct _BSS_INFO_T BSS_INFO_T, *P_BSS_INFO_T;
typedef struct _DEV_INFO_T DEV_INFO_T, *P_DEV_INFO_T;
typedef struct _BSS_INFO_T P2P_DEV_INFO_T, *P_P2P_DEV_INFO_T;
typedef struct _CHNL_INFO_T CHNL_INFO_T, *P_CHNL_INFO_T;
typedef struct _AIS_SPECIFIC_BSS_INFO_T AIS_SPECIFIC_BSS_INFO_T, *P_AIS_SPECIFIC_BSS_INFO_T;
typedef struct _P2P_SPECIFIC_BSS_INFO_T P2P_SPECIFIC_BSS_INFO_T, *P_P2P_SPECIFIC_BSS_INFO_T;
typedef struct _BOW_SPECIFIC_BSS_INFO_T BOW_SPECIFIC_BSS_INFO_T, *P_BOW_SPECIFIC_BSS_INFO_T;

typedef VOID (*IST_EVENT_FUNCTION)(UINT_8, UINT_32);

/*
 * Type definition for function pointer of timer handler
 */
typedef VOID (*PFN_TIMER_CALLBACK)();

typedef VOID (*PFN_TEST_CASE_HANDLER)(VOID);
typedef VOID (*PFN_TEST_CALLBACK_IN_PHY)(UINT_8);

/*
 * Type definition for function pointer of management frame
 */
typedef WLAN_STATUS (*PROCESS_RX_MGT_FUNCTION)(P_SW_RFB_T);

/*
 * Tx Call Back Function
 */
typedef WLAN_STATUS (*PFN_TX_DONE_HANDLER)(P_MSDU_INFO_T, WLAN_STATUS);

typedef struct _BIN_CONTENT_T
{
    UINT_16 u2Addr;
    UINT_8 ucValue;
    UINT_8 ucReserved;
} BIN_CONTENT_T, *P_BIN_CONTENT_T;

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

#endif  /* _TYPEDEF_H */

