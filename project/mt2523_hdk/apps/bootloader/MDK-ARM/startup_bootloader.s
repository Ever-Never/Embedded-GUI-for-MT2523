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

; main stack allocates at end of TCM, which is determined by scatter file
Stack_Size      EQU     0x1000

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp

                PRESERVE8
                THUMB

; Vector Table Mapped to head of RAM
                AREA    |.isr_vector|, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size
                IMPORT  |Image$$STACK$$ZI$$Limit|

__Vectors       DCD     |Image$$STACK$$ZI$$Limit|  ; Top of Stack
                DCD     Reset_Handler              ; Reset Handler


__Vectors_End

__Vectors_Size  EQU  __Vectors_End - __Vectors

                AREA    |.reset_handler|, CODE, READONLY


; Reset handler
Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                EXPORT  JumpCmd
                IMPORT  __main
                IMPORT  |Image$$STACK$$ZI$$Base|
                IMPORT  |Image$$STACK$$ZI$$Limit|

                ;set stack pointer
                LDR     SP, =|Image$$STACK$$ZI$$Limit|

                ;interrupt disable
                CPSID   I

                ;cache disable
                LDR     R0, =0xA0880000
                MOVS    R1, #0
                STR     R1, [R0, #0]

                ;stack space zero init
                MOVS    R0, #0
                LDR     R1, =|Image$$STACK$$ZI$$Base|
                LDR     R2, =|Image$$STACK$$ZI$$Limit|
                BL      FillZero

                ;Call the application's entry point
                LDR     R0, =__main
                BX      R0
                ENDP

FillZero        PROC
                STR     R0, [R1], #4
                CMP     R1, R2
                BCC     FillZero
                BX      LR
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

;                B       .

;                ENDP

                ALIGN

      AREA |INTERNCODE|, CODE, READONLY
; TODO need to check JumpCmd can work on FPGA
JumpCmd
                ORR     r0, #0x01
                BX      r0

;*******************************************************************************
; User Stack and Heap initialization
;*******************************************************************************
                IF      :DEF:__MICROLIB
                EXPORT  __initial_sp
                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap

                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR

                ALIGN

                ENDIF

                END

;************************ (C) COPYRIGHT MEDIATEK *****END OF FILE*****
