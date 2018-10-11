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

/**
 * @file cm4_exception.c
 *
 *  Define the CM4 exception handlers which is used in startup_mt7687.s
 *
 *  Author terrence.mei
 */

#include "exception_mt7687.h"
#include "mt7687.h"
#include <stdint.h>


static fault_finish_mode_t g_fault_finish_mode = FAULT_LOOP;

void set_fault_priority(void)
{
    //NMI & Hardfault are fixed high priority -2 & -1

    //system handler priority register1
    //PRI_4,Menmanage
    //SCB->SHP[0] = 0;
    NVIC_SetPriority(MemoryManagement_IRQn, FAULT_EXCEPTION_PRIORITY);

    //PRI_5,Busfault
    //SCB->SHP[1] = 0;
    NVIC_SetPriority(BusFault_IRQn, FAULT_EXCEPTION_PRIORITY);

    //PRI_6,Usagefault
    //SCB->SHP[2] = 0;
    NVIC_SetPriority(UsageFault_IRQn, FAULT_EXCEPTION_PRIORITY);
}


void enable_fault_exception(void)
{
    //system handler control and state register
    SCB->SHCSR |= 0x40000; //bit18,usage fault enable
    SCB->SHCSR |= 0x20000; //bit17,bus fault enable
    SCB->SHCSR |= 0x10000; //bit16,mem fault enable

    //configuration and control register
    SCB->CCR   |= 0x0010; //DIV_0_TRP
}

void cfsr_status_report(uint32_t cfsr)
{
    if ((cfsr >> MMFSR_SHIFT) & MMFSR_MASK) {
        dbg_fatal("Memory fault status");
        //add defailed info
    } else if ((cfsr >> BFSR_SHIFT) & BFSR_MASK) {
        dbg_fatal("Bus fault status");
        //add defailed info

    } else if ((cfsr >> UFSR_SHIFT) & UFSR_MASK) {
        dbg_fatal("Usage fault status");
        //add defailed info
    }

}

void disable_fault_exception(void)
{
    //system handler control and state register
    SCB->SHCSR &= 0xFFFBFFFF; //bit18,usage fault disable
    SCB->SHCSR &= 0xFFFDFFFF; //bit17,bus fault disable
    SCB->SHCSR &= 0xFFFEFFFF; //bit16,mem fault disable
}


void fault_reset(void)
{
    dbg_fatal("System_Reset");

    //Need MTK provide reboot api,now just use wihle loop instead
    while (1);
}

void fault_dump(uint32_t sp[])
{
    //dump stack,here we dump R0,R1,R2,R3,R12,LR,PC,xPSR.
    dbg_fatal("register dump:\r\n R0=0x%08lx, R1=0x%08lx, R2=0x%08lx, R3=0x%08lx,\r\nR12=0x%08lx, LR=0x%08lx, PC=0x%08lx, xPSR=0x%08lx",
              sp[0], sp[1], sp[2], sp[3], sp[4], sp[5], sp[6], sp[7]);
}

void set_fault_finish_mode(fault_finish_mode_t mode)
{
    if (mode < FAULT_MAX) {
        g_fault_finish_mode = mode;
    } else {
        dbg_info("invalid fault mode:%d", mode);
    }
}

fault_finish_mode_t get_fault_finish_mode(void)
{
    return g_fault_finish_mode;
}


void fault_exception_finish(void)
{
    if (FAULT_RESET == get_fault_finish_mode()) {
        fault_reset();
    } else if (get_fault_finish_mode() == FAULT_LOOP) {
        while (1) {}
    } else {
        while (1) {}
    }
}

/*************************************************************************/
/*************************************************************************/

void NMI_Handler(void)
{
    __asm("TST lr, #4");     //test bit2 of EXC_RETURN
    __asm("ITE EQ");         //check whether EQ is 0
    __asm("MRSEQ r0, MSP");  //if 0,current exception use MSP,then save the stack pointer
    __asm("MRSNE r0, PSP");  //if 1,current exception use PSP,then save the stack pointer
    __asm("B _NMI_Handler"); //go to real exception handler

}

void HardFault_Handler(void)
{
    __asm("TST lr, #4");
    __asm("ITE EQ");
    __asm("MRSEQ r0, MSP");
    __asm("MRSNE r0, PSP");
    __asm("B _HardFault_Handler");

}

void MemManage_Handler(void)
{
    __asm("TST lr, #4");
    __asm("ITE EQ");
    __asm("MRSEQ r0, MSP");
    __asm("MRSNE r0, PSP");
    __asm("B _MemManage_Handler");

}

void BusFault_Handler(void)
{
    __asm("TST lr, #4");
    __asm("ITE EQ");
    __asm("MRSEQ r0, MSP");
    __asm("MRSNE r0, PSP");
    __asm("B _BusFault_Handler");

}

void UsageFault_Handler(void)
{
    __asm("TST lr, #4");
    __asm("ITE EQ");
    __asm("MRSEQ r0, MSP");
    __asm("MRSNE r0, PSP");
    __asm("B _UsageFault_Handler");
}


/*************************************************************************/
/*************************************************************************/

void _NMI_Handler(uint32_t sp[])
{
    dbg_fatal("NMI exception");
    fault_dump(sp);

    /*special handle*/
    fault_exception_finish();
}


void _HardFault_Handler(uint32_t sp[])
{
    dbg_fatal("SCB->HFSR=0x%08lx", SCB->HFSR);

    if (((SCB->HFSR) & (1 << 30)) != 0) {
        dbg_fatal("SCB->CFSR=0x%08lx", SCB->CFSR);
        cfsr_status_report(SCB->CFSR);
    }

    fault_dump(sp);

    /*special handle*/
    fault_exception_finish();
}

void _MemManage_Handler(uint32_t sp[])
{
    dbg_fatal("SCB->CFSR=0x%08lx", SCB->CFSR);
    cfsr_status_report(SCB->CFSR);
    fault_dump(sp);

    /*special handle*/
    fault_exception_finish();
}

void _BusFault_Handler(uint32_t sp[])
{
    dbg_fatal("SCB->CFSR=0x%08lx", SCB->CFSR);
    cfsr_status_report(SCB->CFSR);
    fault_dump(sp);

    /*special handle*/
    fault_exception_finish();
}

void _UsageFault_Handler(uint32_t sp[])
{
    dbg_fatal("SCB->CFSR=0x%08lx", SCB->CFSR);
    cfsr_status_report(SCB->CFSR);
    fault_dump(sp);

    /*special handle*/
    fault_exception_finish();
}







