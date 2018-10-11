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

#include <stdint.h>

#define HW_WRITE(ptr,data)     (*(ptr) = (data))
#define HW_READ(ptr)           (*(ptr))


#define OSTIMER_BASE           0xA01F0000
#define OSTIMER_IRQ            0x11
#define CMD_MAGIC_VALUE        0xFFFF0000
#define OST_MAX_WAIT_TIME      600

#define OST1_BASE              OSTIMER_BASE   /* 0xA01F0000 is used for 6260/6261/2501/2502 */

/* Registers for ARM side OS Timer. */
#define OST_CON                ((volatile uint32_t*)(OST1_BASE+0x00))  /* OS Timer Control Register */
                                  /* 0: EN */
                                  /* 1: UFN_DOWN */
                                  /* 2: OST_DBG: Enable OST wake-up debug function */
#define OST_CMD                ((volatile uint32_t*)(OST1_BASE+0x04))  /* OS Timer Command Register */
                                  /* 0: PAUSE_START */
                                  /* 1: OST_RD */
                                  /* 2: OST_WR */
                                  /* 13: OST_UFN_WR */
                                  /* 14: OST_AFN_WR */
                                  /* 15: OST_CON_WR */
#define OST_STA                ((volatile uint32_t*)(OST1_BASE+0x08))  /* OS Timer Command Status Register  */
                                  /* 0: READY: to indicate OST is in Normal or Pause mode */
                                  /* 1: CMD_CPL: to indicate OST command is completed or not */
                                  /* [4:3]: to indicate a Pause request is received by HW or not */
                                  /* 00: last pause command request is not completed yet */
                                  /* 01: last pause command request is completed with OST pause mode active */
                                  /* 10: last pause command request is completed with wakeup sources */
                                  /* 11: last pause command request is completed with UFN < 2 */
                                  /* 6: AFN_DLY_OVER: to indicate AFN_DLY Counter is overflow or not*/
                                  /* 15: CPU_SLEEP: to indicate the processor is in sleep mode or not (For debug purpose)*/
#define OST_FRM                ((volatile uint16_t*)(OST1_BASE+0x0C))  /* OS Timer Frame Duration [12:0]        */
#define OST_FRM_F32K           ((volatile uint16_t*)(OST1_BASE+0x10))  /* OS Timer Frame Duration by 32K clock [8:0]  */
                                  /* [15:12]: OST_FRM_NUM: set this value if OST_FRM < system settling time */
                                  /* [8:0]: OST_FRM_F32K */
#define OST_UFN                ((volatile uint32_t*)(OST1_BASE+0x14))  /* OS Timer Un-alignment Frame Number [31:0]  */
#define OST_AFN                ((volatile uint32_t*)(OST1_BASE+0x18))  /* OS Timer Alignment Frame Number [31:0] */
#define OST_AFN_DLY            ((volatile uint32_t*)(OST1_BASE+0x1C))  /* OS Timer Alignment Frame Delay Number [31:0]*/
#define OST_UFN_R              ((volatile uint32_t*)(OST1_BASE+0x20))  /* Current OS Timer Un-alignement Frame Number [31:0] */
#define OST_AFN_R              ((volatile uint32_t*)(OST1_BASE+0x24))  /* Current OS Timer Alignement Frame Number [31:0] */
#define OST_INT_MASK           ((volatile uint16_t*)(OST1_BASE+0x30))  /* OS Timer Interrupt Mask (All default masked: disabled)       */
                                  /* 0: mask of OS Timer Frame Time Out interrupt */
                                  /* 1: mask of OS Timer Alignment Frame Time Out interrupt */
                                  /* 2: mask of OS Timer Un-Alignment Frame Time Out interrupt */
                                  /* 3: mask of OS Timer Pause Abort interrupt */
                                  /* 4: mask of OS Timer Pause Interrupt interrupt */
#define OST_ISR                ((volatile uint16_t*)(OST1_BASE+0x40))  /* OS Timer Interrupt Status        */
                                  /* 0: OS Timer Frame Time Out interrupt Status */
                                  /* 1: OS Timer Alignment Frame Time Out interrupt Status */
                                  /* 2: OS Timer Un-Alignment Frame Time Out interrupt Status */
                                  /* 3: OS Timer Pause Abort interrupt Status */
                                  /* 4: OS Timer Pause Interrupt interrupt Status */
#define OST_EVENT_MASK         ((volatile uint32_t*)(OST1_BASE+0x50))  /* OS Timer Event Mask (All default enabled)       */
                                  /* 0: GPT (E1: Edge, E2: Level ??) */
                                  /* 1: Touch Pannel (Level) */
                                  /* 2: Key Pad (Edge) */
                                  /* 3: MSDC1 (E1: Edge, E2: Level ??) */
                                  /* 4: MSDC2 (E1: Edge, E2: Level ??) */
                                  /* 5: MSDC3 (E1: Edge, E2: Level ??) */
                                  /* 6: EINT (Level) */
                                  /* 7: RTC (Level) */
                                  /* 8: BTIF (Edge/Level ?) */
                                  /* 9: Timer Trigger (Edge/Level) */
                                  /* 10: ACCDET [E2 new added] */
                                  /* 11: Reserved */
                                  /* 12: nFIQ (Level) */
                                  /* 13: nIRQ (Level) */
                                  /* 14: MDIF (Level) */
                                  /* 15: EDBRGQ (Level) */
                                  /* 16: ARM CIRQ Wakeup (Level) */
                                  /* 17: OStimer ARM cpu sleep [E2 new added] (could not use: Always Pause Abort) */
                                  /* 18: Reserved */
#define OST_WAKEUP_STA         ((volatile uint32_t*)(OST1_BASE+0x54))  /* OS Timer Event Wakeup Status [auto-clear with source]: record the first wakeup sources       */
#define OST_DBG_WAKEUP         ((volatile uint32_t*)(OST1_BASE+0x60))  /* OS Timer Debug Wakeup        */
                                  /* 31: CIRQ_MASK_EN: 1=enable cirq mask function */


