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
#define WDT_Base            0xA2050000
#define WDT_Disable         0x2200
#define Remap_Base          0xA0881000
#define Remap_Entry_HI0     0x10000023
#define Remap_Entry_LO0     0x0


        MODULE  ?cstartup

        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:DATA:ROOT(2)

        EXTERN  __iar_program_start
        EXTERN  CachePreInit
        EXTERN  SystemInit
        EXTERN  Flash_ReturnReady
        PUBLIC  __vector_table

__iar_init$$done:               ; The vector table is not needed
                                ; until after copy initialization is done

        DATA
__vector_table

        DCD     SFE(CSTACK)                ; Top of Stack
        DCD     Reset_Handler              ; Reset Handler
        DCD     NMI_Handler                ; NMI Handler
        DCD     HardFault_Handler          ; Hard Fault Handler
        DCD     MemManage_Handler          ; MPU Fault Handler
        DCD     BusFault_Handler           ; Bus Fault Handler
        DCD     UsageFault_Handler         ; Usage Fault Handler
        DCD     0                          ; Reserved
        DCD     0                          ; Reserved
        DCD     0                          ; Reserved
        DCD     0                          ; Reserved
        DCD     SVC_Handler               ; SVCall Handler
        DCD     DebugMon_Handler           ; Debug Monitor Handler
        DCD     0                          ; Reserved
        DCD     PendSV_Handler                ; PendSV Handler
        DCD     SysTick_Handler               ; SysTick Handler

        ; External Interrupts
        DCD     isrC_main                  ;16:  OS timer
        DCD     isrC_main     			   ;17:  Reserved
        DCD     isrC_main     			   ;18:  Modem TOPSM
        DCD     isrC_main                  ;19:  CM4 TOPSM
        DCD     isrC_main                  ;20:  Reserved
        DCD     isrC_main                  ;21:  Reserved
        DCD     isrC_main                  ;22:  Reserved
        DCD     isrC_main                  ;23:  Reserved
        DCD     isrC_main                  ;24:  Accessory Detector
        DCD     isrC_main                  ;25:  RTC
        DCD     isrC_main                  ;26:  Keypad
        DCD     isrC_main                  ;27:  General Purpose Timer
        DCD     isrC_main                  ;28:  External Interrupt
        DCD     isrC_main                  ;29:  LCD
        DCD     isrC_main                  ;30:  LCD AAL
        DCD     isrC_main                  ;31:  DSI
        DCD     isrC_main                  ;32:  RESIZE
        DCD     isrC_main                  ;33:  Graphics 2D
        DCD     isrC_main                  ;34:  Camera
        DCD     isrC_main                  ;35:  ROT DMA
        DCD     isrC_main                  ;36:  SCAM
        DCD     isrC_main                  ;37:  DMA
        DCD     isrC_main                  ;38:  DMA always on
        DCD     isrC_main                  ;39:  I2C dual
        DCD     isrC_main                  ;40:  I2C0
        DCD     isrC_main                  ;41:  I2C1
        DCD     isrC_main                  ;42:  I2C2
        DCD     isrC_main                  ;43:  Reserved
        DCD     isrC_main                  ;44:  General Purpose Counter
        DCD     isrC_main                  ;45:  UART0
        DCD     isrC_main                  ;46:  UART1
        DCD     isrC_main                  ;47:  UART2
        DCD     isrC_main                  ;48:  UART3
        DCD     isrC_main                  ;49:  USB2.0
        DCD     isrC_main                  ;50:  MSDC0
        DCD     isrC_main                  ;51:  MSDC1
        DCD     isrC_main                  ;52:  Reserved
        DCD     isrC_main                  ;53:  Reserved
        DCD     isrC_main                  ;54:  Serial FLASH
        DCD     isrC_main                  ;55:  DSP22CPU
        DCD     isrC_main                  ;56:  Sensor DMA
        DCD     isrC_main                  ;57:  RGU
        DCD     isrC_main                  ;58:  SPI Slave
        DCD     isrC_main                  ;59:  SPI Master0
        DCD     isrC_main                  ;60:  SPI Master1
        DCD     isrC_main                  ;61:  SPI Master2
        DCD     isrC_main                  ;62:  SPI Master3
        DCD     isrC_main                  ;63:  TRNG
        DCD     isrC_main                  ;64:  Bluetooth TIMCON
        DCD     isrC_main                  ;65:  Bluetooth IF
        DCD     isrC_main                  ;66:  Reserved
        DCD     isrC_main                  ;67:  Reserved
        DCD     isrC_main                  ;68:  LISR0
        DCD     isrC_main                  ;69:  LISR1
        DCD     isrC_main                  ;70:  LISR2
        DCD     isrC_main                  ;71:  LISR3
        DCD     isrC_main                  ;72:  PSI Rise Edge
        DCD     isrC_main                  ;73:  PSI Fall Edge
        DCD     isrC_main                  ;74:  Reserved
        DCD     isrC_main                  ;75:  Reserved
        DCD     isrC_main                  ;76:  PSI Fall Edge
        DCD     isrC_main                  ;77:  Reserved
        DCD     isrC_main                  ;78:  Reserved
        DCD     isrC_main                  ;79:  Reserved

        THUMB
        PUBWEAK Reset_Handler
        SECTION .reset_handler:CODE:REORDER:ROOT(2)
Reset_Handler
        ;set stack pointer
        LDR     SP, =SFE(CSTACK)

        ;interrupt disable
        CPSID   I

        ;preinit cache to accelerate region init progress
        LDR     R0, =CachePreInit
        BLX     R0

        ;watch dog disable
        LDR     R0, =WDT_Base
        LDR     R1, =WDT_Disable
        STR     R1, [R0, #0]

        ;make virtual space available
        LDR     R0, =Remap_Entry_HI0
        LDR     R1, =Remap_Entry_LO0
        LDR     R2, =Remap_Base
        STR     R0, [R2], #4
        STR     R1, [R2, #0]

        ;stack space zero init
        MOVS    R0, #0
        LDR     R1, =SFB(CSTACK)
        LDR     R2, =SFE(CSTACK)
FillZero
        STR     R0, [R1], #4
        CMP     R1, R2
        BCC     FillZero

        ;Call system intitialization function
        LDR     R0, =SystemInit
        BLX     R0

        ;Call the application's entry point
        LDR     R0, =__iar_program_start
        BX      R0


        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
        B HardFault_Handler

        PUBWEAK MemManage_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
MemManage_Handler
        B MemManage_Handler

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BusFault_Handler
        B BusFault_Handler

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
UsageFault_Handler
        B UsageFault_Handler

        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        B SVC_Handler

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
DebugMon_Handler
        B DebugMon_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B SysTick_Handler


        PUBWEAK isrC_main
        SECTION .ram_code:CODE:REORDER:NOROOT(2)
isrC_main
        B       .

        END
;************************ (C) COPYRIGHT MEDIATEK *****END OF FILE*****
