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

#ifndef __FLASH_MTD_SF_UT_H__
#define __FLASH_MTD_SF_UT_H__

#include "hal_flash_opt.h"
#include "hal_flash.h"

#if (!defined(__UBL__))
#if ( (defined(__NOR_FLASH_HARDWARE_TEST__) && !defined(__UBL__)))
#ifdef __SERIAL_FLASH__
#define SF_DEBUG
#endif
#endif
#endif //!defined(__UBL__)

// Test Setup
//-----------------------------------------------------------------------------
// [Test Cases]
//    SFUT_TEST_ERASE_PROGRAM_WITHOUT_SUSPEND
//      - Erase / Program without suspends
//    SFUT_TEST_ERASE_PROGRAM_WITH_SUSPEND
//      - Erase / Program with suspends
//    SFUT_TEST_MACRO_READ
//      - Macro Mode read test, regarding to the CS high time.
//    SFUT_TEST_WRAP_READ
//      - Wrap read functional test
//    SFUT_TEST_PROG_DISTURBANCE
//      - Program disturbance test
//      - Suspends only in program operation, after program, verifies all data within the same block.
//    SFUT_TEST_Erase_DISTURBANCE
//      - Erase disturbance test
//      - Suspends only in erase operation, after erase, verifies all data within the same block-group (256KB).
//
// [Configruations]
//    SFUT_DISTURBANCE_CHECK_IN_LISR
//      - Enable distrubance checking in SW intrrupt LISR
//      - For debug purpose only, target for the suspend-read fail.
//      - This will significantly slowing down the test speed.
//    SFUT_DISTURBANCE_LOOP_LIMIT
//      - Loop limit in erase/program disturnce test (N x 256KB)
//    SFUT_ERASE_SUSPEND_FREQUENCY
//      - SW interrupt (suspend) frequency while erasing, in terms of qbits per interrupt.
//      - Note: For some vendors, intense erase-suspend will cause erase time over expectation.
//          *** If the test hanged in the erase suspend test, try to set the frqeuncy to 4096.
//    SFUT_PROG_SUSPEND_FREQUENCY
//      - SW interrupt (suspend) frequency while programming, in terms of qbits per interrupt.
//      - Note: For some vendors, intense program-suspend will cause program time over expectation.
//          *** If the test hanged in the program suspend test, try to set the frqeuncy to 1024.
//    SFUT_CSHT_MIN, SFUT_CSHT_MAX, SFUT_CSHT_STEP
//      - The range and step of the CS high time
//      - The three parameters are used in Macro mode read test (SFUT_TEST_MACRO_READ)
//    SFUT_WRAP_LEN
//      - Warp read length
//    SFUT_WRAP_TEST_COUNT
//      - Number of random wrap read test counts
//-----------------------------------------------------------------------------


// Basic Load Functional Test
//---------------------------------
// Test Cases
//#define SFUT_TEST_LOW_HIGH_FQ_SWITCH
#define SFUT_TEST_DPD
#define SFUT_TEST_ERASE_PROGRAM_WITHOUT_SUSPEND
#define SFUT_TEST_ERASE_PROGRAM_WITH_SUSPEND // mt6255 fpga
#define SFUT_TEST_MACRO_READ // mt6255 fpga
#define SFUT_TEST_WRAP_READ // mt6255 fpga
#define SFUT_TEST_ERASE_DISTURBANCE // mt6255 fpga
#define SFUT_TEST_PROG_DISTURBANCE // mt6255 fpga
//-----------------------------
// Suspend Frequency
#define SFUT_ERASE_SUSPEND_FREQUENCY 4096
#define SFUT_PROG_SUSPEND_FREQUENCY  128
// Macro Mode Read Test: Chip Select High Time
#define SFUT_CSHT_MIN  100
#define SFUT_CSHT_MAX  500
#define SFUT_CSHT_STEP 10
// Macro Mode Read Test: Test Patterns
#define SFUT_MAC_READ_COMPACT
// Wrap Read Test Parameters
#define SFUT_WRAP_LEN 32
#define SFUT_WRAP_TEST_COUNT 4096
// Disturbance Test Parameters
//  #define SFUT_DISTURBANCE_CHECK_IN_LISR
#define SFUT_DISTURBANCE_LOOP_LIMIT 0xFFFFFFFF
#define SFUT_DISTURBANCE_TIME_LIMIT 4
// Enable Debug Log
#define SFUT_DEBUG_LOG
#define SFUT_SHOW_TEST_COVERAGE
// Suspend Test
#define SFUT_SUSPEND_TEST_ITERATION 4

//-----------------------------------------------------------------------------

// Exported Serial Flash Unit Test APIs
bool SFUT_DeviceTest(bool system_init);
void SFUT_LISR(void);
void SFUT_LISRInsert(void);
void SFUT_IntTrigger(void);
void SFUT_CorruptNOR(void);

// SW Interrupt Frequency Control
extern uint32_t SFUT_IntTimeCurr;
extern uint32_t SFUT_IntTimePrev;
extern uint32_t SFUT_IntTimeDiff;
extern uint32_t SFUT_IntFreq;
extern uint32_t SFUT_IntBusySuspend;
extern uint32_t SFUT_IntSuspendBase;


#define SFUT_READY_WRAP_FUNCTION_LED_GPIO     (47)    // Combinational read test
#define SFUT_READY_BASIC_FUNCTION_LED_GPIO    (49)    // Program / Erase without suspend
#define SFUT_READY_SUSPEND_FUNCTION_LED_GPIO  (51)    // Program / Erase with suspend + disturbance check

// SFUT concurrency test (only for Memory SLT)
// #define SFUT_MSDC_TRACE_ENABLE
//#define SFUT_MSDC_CONCURRENCY_TEST

// Erase suspend disable
typedef enum {
    SFUT_DISABLE_ERASE_SUSPEND = 0x1,
    SFUT_DISABLE_PROG_SUSPEND = 0x2
} Enum_SFUT_DISABLE_SUSPEND;

extern uint32_t sfut_suspend_enable;




// Interrupt (Suspend) Insertion Macro
#if defined(SF_DEBUG)
// TODO: Add randomizer here
#define SFUT_IntTest(triggerfunc)   do {  \
    if (0!=SFUT_IntFreq) { \
        SFUT_IntTimeCurr=ust_get_current_time();  \
        if (ust_get_duration(SFUT_IntTimePrev, SFUT_IntTimeCurr) > (SFUT_IntFreq)) {  \
            SFUT_IntTimePrev=SFUT_IntTimeCurr; triggerfunc(); \
        } \
    } \
    if (0<SFUT_IntBusySuspend) { \
		SFUT_IntTimeCurr=ust_get_current_time(); \
		ust_busy_wait(SFUT_IntBusySuspend-1); \
		triggerfunc(); \
    } \
 } while(0)
#else
#define SFUT_IntTest(...)
#endif

#endif // __FLASH_MTD_SF_UT_H__

