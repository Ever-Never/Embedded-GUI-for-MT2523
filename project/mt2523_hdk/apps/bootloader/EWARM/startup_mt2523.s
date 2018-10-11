;/* Copyright Statement:
; *
; * (C) 2005-2016  MediaTek Inc. All rights reserved.
; *
; * This software/firmware and related documentation ("MediaTek Software") are
; * protected under relevant copyright laws. The information contained herein
; * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
; * Without the prior written permission of MediaTek and/or its licensors,
; * any reproduction, modification, use or disclosure of MediaTek Software,
; * and information contained herein, in whole or in part, shall be strictly prohibited.
; * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
; * if you have agreed to and been bound by the applicable license agreement with
; * MediaTek ("License Agreement") and been granted explicit permission to do so within
; * the License Agreement ("Permitted User").  If you are not a Permitted User,
; * please cease any access or use of MediaTek Software immediately.
; * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
; * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
; * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
; * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
; * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
; * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
; * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
; * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
; * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
; * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
; * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
; * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
; * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
; * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
; * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
; * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
; * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
; */

; Macro defines
#define Cache_Ctrl_Base     0xA0880000
#define Cache_Disable       0x0
#define Cache_Invalid       0x3
#define WDT_Base            0xA2050000
#define WDT_Disable         0x2200
#define Remap_Base          0xA0881000
#define Remap_Entry_HI0     0x10000023
#define Remap_Entry_LO0     0x0


        MODULE  ?cstartup

        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:DATA:NOROOT(2)

        EXTERN  __iar_program_start
        EXPORT  JumpCmd
        PUBLIC  __vector_table

__iar_init$$done:               ; The vector table is not needed
                                ; until after copy initialization is done

        DATA
__vector_table

        DCD     SFE(CSTACK)                ; Top of Stack
        DCD     Reset_Handler              ; Reset Handler

        THUMB
        PUBWEAK Reset_Handler
        SECTION .reset_handler:CODE:REORDER:NOROOT(2)

Reset_Handler
        ;set stack pointer
        LDR     SP, =SFE(CSTACK)

        ;interrupt disable
        CPSID   I

        ;cache disable, make region init safer
        MOVS    R0, #Cache_Disable
        ;MOVS    R1, #Cache_Invalid
        LDR     R2, =Cache_Ctrl_Base
        STR     R0, [R2], #4
        ;STR     R1, [R2, #0]

        ;watch dog disable
        ;LDR     R0, =WDT_Base
        ;LDR     R1, =WDT_Disable
        ;STR     R1, [R0, #0]

        ;make virtual space available
        ;LDR     R0, =Remap_Entry_HI0
        ;LDR     R1, =Remap_Entry_LO0
        ;LDR     R2, =Remap_Base
        ;STR     R0, [R2], #4
        ;STR     R1, [R2, #0]

        ;stack space zero init
        MOVS    R0, #0
        LDR     R1, =SFB(CSTACK)
        LDR     R2, =SFE(CSTACK)
FillZero
        STR     R0, [R1], #4
        CMP     R1, R2
        BCC     FillZero

        LDR     R0, =__iar_program_start
        BLX     R0


JumpCmd
        ORR     R0,R0, #0x01
        BX      R0

        END
;************************ (C) COPYRIGHT MEDIATEK *****END OF FILE*****
