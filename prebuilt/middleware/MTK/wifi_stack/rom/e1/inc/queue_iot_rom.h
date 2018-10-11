/*
** $Id:
*/

/*! \file   queue.h
    \brief  Definition for singly queue operations.

    In this file we define the singly queue data structure and its
    queue operation MACROs.
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
** $Log: queue.h $
 *
 * 05 21 2015 raghav.monga
 * [BORA00004254] [MT7637E2][IOT][FW]
 * Remove RAM header file dependancy in ROM code.
 * Created parallel header files for RAM files which were being used in ROM code.
** 
** 10 23 2012 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Avoid duplicated definition or type in ROM/RAM header files
**
** 10 04 2012 yarco.yang
** [BORA00002189] [MT6630]Synch. Davinc 20120927 FW to Bora
** .
*/

#ifndef _QUEUE_H
#define _QUEUE_H


/*******************************************************************************
*							   C O N S T A N T S
********************************************************************************
*/

/*******************************************************************************
*							  D A T A	T Y P E S
********************************************************************************
*/
/* Singly Queue Structures - Entry Part */
typedef struct _QUE_ENTRY_T {
	struct _QUE_ENTRY_T *prNext;
} QUE_ENTRY_T, *P_QUE_ENTRY_T;

/* Singly Queue Structures - Queue Part */
typedef struct _QUE_T {
	P_QUE_ENTRY_T	prHead;
	P_QUE_ENTRY_T	prTail;
	UINT_32 		u4NumElem;
} QUE_T, *P_QUE_T;


/*******************************************************************************
*							 P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*							P R I V A T E	D A T A
********************************************************************************
*/

/*******************************************************************************
*								  M A C R O S
********************************************************************************
*/
#define QUEUE_INITIALIZE(prQueue) \
			{ \
				(prQueue)->prHead = (P_QUE_ENTRY_T)NULL; \
				(prQueue)->prTail = (P_QUE_ENTRY_T)NULL; \
				(prQueue)->u4NumElem = 0; \
			}
	
#define QUEUE_IS_EMPTY(prQueue)             (((P_QUE_T)(prQueue))->prHead == (P_QUE_ENTRY_T)NULL)
	
#define QUEUE_IS_NOT_EMPTY(prQueue)         ((prQueue)->u4NumElem > 0)
	
#define QUEUE_GET_HEAD(prQueue)             ((prQueue)->prHead)
	
#define QUEUE_GET_TAIL(prQueue)             ((prQueue)->prTail)
	
#define QUEUE_GET_NEXT_ENTRY(prQueueEntry)  ((prQueueEntry)->prNext)
	
#define QUEUE_INSERT_HEAD(prQueue, prQueueEntry) \
			{ \
				LITE_ASSERT(prQueue); \
				LITE_ASSERT(prQueueEntry); \
				(prQueueEntry)->prNext = (prQueue)->prHead; \
				(prQueue)->prHead = (prQueueEntry); \
				if ((prQueue)->prTail == (P_QUE_ENTRY_T)NULL) { \
					(prQueue)->prTail = (prQueueEntry); \
				} \
				((prQueue)->u4NumElem)++; \
			}
	
#define QUEUE_INSERT_TAIL(prQueue, prQueueEntry) \
			{ \
				LITE_ASSERT(prQueue); \
				LITE_ASSERT(prQueueEntry); \
				(prQueueEntry)->prNext = (P_QUE_ENTRY_T)NULL; \
				if ((prQueue)->prTail) { \
					((prQueue)->prTail)->prNext = (prQueueEntry); \
				} else { \
					(prQueue)->prHead = (prQueueEntry); \
				} \
				(prQueue)->prTail = (prQueueEntry); \
				((prQueue)->u4NumElem)++; \
			}
	
	/* NOTE: We assume the queue entry located at the beginning of "prQueueEntry Type",
	 * so that we can cast the queue entry to other data type without doubts.
	 * And this macro also decrease the total entry count at the same time.
	 */
#define QUEUE_REMOVE_HEAD(prQueue, prQueueEntry, _P_TYPE) \
			{ \
				LITE_ASSERT(prQueue); \
				prQueueEntry = (_P_TYPE)((prQueue)->prHead); \
				if (prQueueEntry) { \
					(prQueue)->prHead = ((P_QUE_ENTRY_T)(prQueueEntry))->prNext; \
					if ((prQueue)->prHead == (P_QUE_ENTRY_T)NULL){ \
						(prQueue)->prTail = (P_QUE_ENTRY_T)NULL; \
					} \
					((P_QUE_ENTRY_T)(prQueueEntry))->prNext = (P_QUE_ENTRY_T)NULL; \
					((prQueue)->u4NumElem)--; \
				} \
			}
	
#define QUEUE_MOVE_ALL(prDestQueue, prSrcQueue) \
			{ \
				LITE_ASSERT(prDestQueue); \
				LITE_ASSERT(prSrcQueue); \
				*(P_QUE_T)prDestQueue = *(P_QUE_T)prSrcQueue; \
				QUEUE_INITIALIZE(prSrcQueue); \
			}
	
#define QUEUE_CONCATENATE_QUEUES(prDestQueue, prSrcQueue) \
			{ \
				LITE_ASSERT(prDestQueue); \
				LITE_ASSERT(prSrcQueue); \
				if ((prDestQueue)->prTail) { \
					((prDestQueue)->prTail)->prNext = (prSrcQueue)->prHead; \
				} else { \
					(prDestQueue)->prHead = (prSrcQueue)->prHead; \
				} \
				(prDestQueue)->prTail = (prSrcQueue)->prTail; \
				((prDestQueue)->u4NumElem) += ((prSrcQueue)->u4NumElem); \
				QUEUE_INITIALIZE(prSrcQueue); \
			}
	
	
	/*******************************************************************************
	*				   F U N C T I O N	 D E C L A R A T I O N S
	********************************************************************************
	*/
	
	/*******************************************************************************
	*							   F U N C T I O N S
	********************************************************************************
	*/

#endif /* _QUEUE_H */

