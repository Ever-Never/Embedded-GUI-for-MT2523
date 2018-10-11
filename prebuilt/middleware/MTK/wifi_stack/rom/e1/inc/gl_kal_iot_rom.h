/*
** $Id: //Department/DaVinci/TRUNK/MT6620_WiFi_Firmware/mcu/wifi/include/gl_kal.h#4 $
*/

/*! \file   gl_kal.h
    \brief  Declaration of KAL functions - kal*() which is provided by GLUE Layer.

    Any definitions in this file will be shared among GLUE Layer and internal Driver Stack.
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
** $Log: gl_kal.h $
 *
 * 05 21 2015 raghav.monga
 * [BORA00004254] [MT7637E2][IOT][FW]
 * Remove RAM header file dependancy in ROM code.
 * Created parallel header files for RAM files which were being used in ROM code.
**
** 01 21 2014 ted.huang
** [BORA00002807] [MT7636]
** WMT command/event over WIFI HIF.
** 
** 10 23 2012 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Avoid duplicated definition or type in ROM/RAM header files
**
** 10 04 2012 yarco.yang
** [BORA00002189] [MT6630]Synch. Davinc 20120927 FW to Bora
*/

#ifndef _GL_KAL_H
#define _GL_KAL_H

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#ifdef _lint
typedef UINT_32     task_indx_type;
typedef UINT_32     cos_timer_type;

/* NOTE: put external reference for LINT here */
#define INDX_WIFI           1  /* Define in system\config\include\cos_config.h*/
#define IRQ_WIFI_CODE       11  /* Define in system\init\include\intrCtrl.h */
#define HIF_base            0   /* Define in system\init\include\reg_base.h */
#define MSG_ID_WIFI_RESET   1

cos_timer_type* cos_create_timer(task_indx_type dest_id);
void cos_free_timer(void* timer_ptr);
void cos_start_timer(cos_timer_type *timer, kal_uint32 timeout);
void cos_stop_timer(cos_timer_type *timer);
#endif /* end of _lint */


#if (CFG_WMT_CMD_OVER_WIFI == 1)
/* NOTE: the message will be merged to ROM code on next chip */
#define MSG_ID_WMT_CMD_OVER_WIFI            (MSG_ID_WIFI_EMU_START + 1)
#define MSG_ID_WMT_EVENT_OVER_WIFI          (MSG_ID_WIFI_EMU_START + 2)
#endif


/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
typedef cos_timer_type      KAL_TIMER_HANDLE_T, *P_KAL_TIMER_HANDLE_T;

typedef task_indx_type      KAL_TASK_IDX_T;
typedef msg_type            KAL_MSG_ID_T;

typedef ilm_struct          KAL_MSG_T, *P_KAL_MSG_T;

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

/* OS message related */
#define KAL_GET_MSG_ID(prOsMsg)         ((prOsMsg)->msg_id)
#define KAL_GET_MSG_DATA(prOsMsg)       ((prOsMsg)->data)

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/
int
kalWiFiSendMsg (
    KAL_TASK_IDX_T      rTaskIdx,
    KAL_MSG_ID_T        rMsgId,
    UINT_32             u4Data
    );

#define wifi_send_msg(rTaskIdx, rMsgId, u4Data) \
        kalWiFiSendMsg(rTaskIdx, rMsgId, u4Data)

#define kalDequeMsg(rTaskIdx)           cos_deque_msg(rTaskIdx)
#define kalFreeMsg(prOsMsg)             cos_free_msg(prOsMsg)
#define kalGetMsgQueSize(rTaskIdx)      cos_get_queue_msg_size(rTaskIdx)


/* Timer related functions */
#define kalCreateTimer(taskId)          cos_create_timer(taskId)
#define kalFreeTimer(prTimerHandle)     cos_free_timer(prTimerHandle)

#define kalStartTimer(prTimerHandle, rSysTimeout) \
        cos_start_timer(prTimerHandle, SYSTIME_TO_MSEC(rSysTimeout))

#define kalStopTimer(prTimerHandle)     cos_stop_timer(prTimerHandle)

#endif /* _GL_KAL_H */


