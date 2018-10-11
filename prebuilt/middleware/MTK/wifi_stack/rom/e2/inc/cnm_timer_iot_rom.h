/*
** $Id: //Department/DaVinci/TRUNK/MT6620_WiFi_Firmware/mcu/wifi/include/mgmt/cnm_timer.h#5 $
*/

/*! \file   cnm_timer.h
    \brief  Declaration of timer obj and related timer macro for setup time out
            event.

    In this file we declare the timer object and provide several macro for
    Protocol functional blocks to setup their own time out event.
*/

/*******************************************************************************
* Copyright (c) 2009 MediaTek Inc.
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
** $Log: cnm_timer.h $
 *
 * 05 21 2015 raghav.monga
 * [BORA00004254] [MT7637E2][IOT][FW]
 * Remove RAM header file dependancy in ROM code.
 * Created parallel header files for RAM files which were being used in ROM code.
**
** 01 08 2014 ted.huang
** [BORA00002807] [MT7636]
** Merge 6630E2 RAM code.
**
** 10 22 2013 terry.wu
** [BORA00002207] [MT6630 Wi-Fi] TXM & MQM Implementation
** 1. Fix time related macro overflow issue
** 2. Add an option to use micro second as debug log prefix
**
** 05 24 2013 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Refine macro of SYSTIME_TO_USEC()
**
** 10 23 2012 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Avoid duplicated definition or type in ROM/RAM header files
**
** 10 01 2012 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Draft version with new BSS_INFO, NETWORK_TYPE_INDEX, pkt/mem alloc/free and so on
*/

#ifndef _CNM_TIMER_H
#define _CNM_TIMER_H

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
#undef MSEC_PER_SEC
#define MSEC_PER_SEC                    1000

#undef USEC_PER_MSEC
#define USEC_PER_MSEC                   1000

#define USEC_PER_TU                     1024 /* microsecond */

#ifndef USEC_PER_SEC
#define USEC_PER_SEC                    1000000
#endif

#define MSEC_PER_MIN                    (60 * MSEC_PER_SEC)


#define MGMT_MAX_TIMEOUT_INTERVAL       ((UINT_32)0x7fffffff)


/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

typedef struct _ROOT_TIMER_T {
    LINK_T                  rLinkHead;
    P_KAL_TIMER_HANDLE_T    prTimerHandle;
    OS_SYSTIME              rNextExpiredSysTime;
} ROOT_TIMER_T, *P_ROOT_TIMER_T;


typedef VOID (*PFN_MGMT_TIMEOUT_FUNC)(UINT_32);

typedef struct _TIMER_T {
    LINK_ENTRY_T            rLinkEntry;
    OS_SYSTIME              rExpiredSysTime;
    UINT_16                 u2Minutes;
    UINT_16                 u2Reserved;
    UINT_32                 u4Data;
    PFN_MGMT_TIMEOUT_FUNC   pfMgmtTimeOutFunc;
} TIMER_T, *P_TIMER_T;

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/
extern ROOT_TIMER_T         rRootTimer;

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/
/* Check if time "a" is before time "b" */
/* In 32-bit variable, 0x00000001~0x7fffffff -> positive number,
 *                     0x80000000~0xffffffff -> negative number
 */
#define TIME_BEFORE_64bit(a,b)       (a < b)

#define TIME_BEFORE(a,b)        ((UINT_32)((UINT_32)(a) - (UINT_32)(b)) > 0x7fffffff)

/* #define TIME_BEFORE(a,b)        ((INT_32)((INT_32)(b) - (INT_32)(a)) > 0)
 * may cause UNexpect result between Free build and Check build for WinCE
 */

#define TIME_AFTER(a,b)         TIME_BEFORE(b,a)


/* The macros to convert OS system time */
#if (KAL_HZ <= MSEC_PER_SEC) && !(MSEC_PER_SEC % KAL_HZ)
    #define SYSTIME_TO_MSEC(_systime)           ((_systime) * (MSEC_PER_SEC / KAL_HZ))
    #define MSEC_TO_SYSTIME(_msec)              ( ( ((_msec) + (MSEC_PER_SEC / KAL_HZ)) - 1) / \
                                                 (MSEC_PER_SEC / KAL_HZ))
#elif (KAL_HZ > MSEC_PER_SEC) && !(KAL_HZ % MSEC_PER_SEC)
    #define SYSTIME_TO_MSEC(_systime)           (((_systime) + (KAL_HZ / MSEC_PER_SEC) - 1) / \
                                                 (KAL_HZ / MSEC_PER_SEC))
    #define MSEC_TO_SYSTIME(_msec)              ((_msec) * (KAL_HZ / MSEC_PER_SEC))
#else
    #define SYSTIME_TO_MSEC(_systime)           (UINT_32)((((UINT_64)(_systime) * MSEC_PER_SEC) + KAL_HZ - 1) / KAL_HZ)
    #define MSEC_TO_SYSTIME(_msec)              (UINT_32)(((UINT_64)(_msec) * KAL_HZ + MSEC_PER_SEC - 1) / \
                                                 MSEC_PER_SEC)
#endif

#define SYSTIME_TO_SEC(_systime)            ((_systime) / KAL_HZ)
#define SEC_TO_SYSTIME(_sec)                ((_sec) * KAL_HZ)


/* The macros to convert second & millisecond */
#define MSEC_TO_SEC(_msec)                  ((_msec) / MSEC_PER_SEC)
#define SEC_TO_MSEC(_sec)                   ((UINT_32)(_sec) * MSEC_PER_SEC)


/* The macros to convert millisecond & microsecond */
#define USEC_TO_MSEC(_usec)                 (((_usec) + USEC_PER_MSEC - 1) / USEC_PER_MSEC)
#define MSEC_TO_USEC(_msec)                 ((UINT_32)(_msec) * USEC_PER_MSEC)


/* The macros to convert TU & microsecond, TU & millisecond */
#define TU_TO_USEC(_tu)                     ((_tu) * USEC_PER_TU)
#define TU_TO_MSEC(_tu)                     USEC_TO_MSEC( TU_TO_USEC(_tu) )
#define USEC_TO_TU(_usec)                   (((_usec) + USEC_PER_TU - 1) / USEC_PER_TU)


/* The macros to convert TU & & OS system time, round up by 0.5 */
#define TU_TO_SYSTIME(_tu)                  MSEC_TO_SYSTIME( TU_TO_MSEC(_tu) )
#define SYSTIME_TO_TU(_systime)             \
    ((SYSTIME_TO_USEC(_systime) + ((USEC_PER_TU / 2) - 1)) / USEC_PER_TU)


/* The macros to convert OS system time & microsecond */
#define SYSTIME_TO_USEC(_systime)           (UINT_32)((((UINT_64)(_systime) * USEC_PER_SEC) + KAL_HZ - 1) / KAL_HZ)

/* The macro is similar with MSEC_TO_SYSTIME. But overflow is more easier */
#ifndef USEC_TO_SYSTIME
#define USEC_TO_SYSTIME(_usec)      (UINT_32)(((UINT_64)(_usec) * KAL_HZ + USEC_PER_SEC - 1) / \
                                     USEC_PER_SEC)
#endif

/* The macro to get the current OS system time */
#define GET_CURRENT_SYSTIME(_systime_p)     {*(_systime_p) = kalGetTimeTick();}

/* The macro to copy the system time */
#define COPY_SYSTIME(_destTime, _srcTime)   (_destTime) = (_srcTime)

/* The macro to get the system time difference between t1 and t2 (t1 - t2) */
/* #define GET_SYSTIME_DIFFERENCE(_time1, _time2, _diffTime) \
        (_diffTime) = (_time1) - (_time2) */

/* The macro to check for the expiration, if TRUE means _currentTime >= _expirationTime */
#define CHECK_FOR_EXPIRATION(_currentTime, _expirationTime) \
        ( ((UINT_32)(_currentTime) - (UINT_32)(_expirationTime)) <= 0x7fffffffUL)

/* The macro to check for the timeout */
#define CHECK_FOR_TIMEOUT(_currentTime, _timeoutStartingTime, _timeout) \
    CHECK_FOR_EXPIRATION((_currentTime), ((_timeoutStartingTime) + (_timeout)))

/* The macro to set the expiration time with a specified timeout *//* Watch out for round up.*/
#define SET_EXPIRATION_TIME(_expirationTime, _timeout) \
        { \
            GET_CURRENT_SYSTIME(&(_expirationTime)); \
            (_expirationTime) += (OS_SYSTIME)(_timeout); \
        }

#define timerRenewTimer(adapter,tmr,interval) \
        timerStartTimer(adapter,tmr,interval,(tmr)->function,(tmr)->data)

#define MGMT_INIT_TIMER(_adapter_p, _timer, _callbackFunc) \
        timerInitTimer(_adapter_p, &(_timer), (UINT_32)(_callbackFunc))


/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
VOID
cnmTimerInitialize (
    task_indx_type              eTaskId
    );

VOID
cnmTimerDestroy (
    VOID
    );

VOID
cnmTimerInitTimer (
    IN P_TIMER_T                prTimer,
    IN PFN_MGMT_TIMEOUT_FUNC    pfFunc,
    IN UINT_32                  u4Data
    );

VOID
cnmTimerStopTimer (
    IN P_TIMER_T                prTimer
    );

VOID
cnmTimerStartTimer (
    IN P_TIMER_T                prTimer,
    IN UINT_32                  u4TimeoutMs
    );

VOID
cnmTimerDoTimeOutCheck (
    VOID
    );

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/
__KAL_INLINE__
INT_32
timerPendingTimer (
    IN P_TIMER_T prTimer
    )
{
    return prTimer->rLinkEntry.prNext != NULL;
}

#endif /* _CNM_TIMER_H */


