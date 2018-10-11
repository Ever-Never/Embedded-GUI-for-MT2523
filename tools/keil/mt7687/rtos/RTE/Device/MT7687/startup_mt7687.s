; ******************************************************************************
; * @file      startup_mt7687.s
; * @author    Hugh Chen, MediaTek MCD/IOT team
; * @version   V1.0
; * @date      2015-07-15
; * @brief     MT7687 startup for ARM RVDS toolchain. 
; *
; ******************************************************************************

; main stack allocates at end of TCM, which is determined by scatter file
Stack_Size      EQU     0xC00

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp

; fake Heap, to make compilation pass on KEIL MDK
; real heap allocates at ZI space
Heap_Size       EQU     0x8

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB

; Vector Table Mapped to head of RAM
                AREA    |.isr_vector|, DATA, READONLY
                EXPORT  __isr_vector
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__isr_vector    DCD     __initial_sp               ; Top of Stack
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
                DCD     SVC_Handler                ; SVCall Handler
                DCD     DebugMon_Handler           ; Debug Monitor Handler
                DCD     0                          ; Reserved
                DCD     PendSV_Handler             ; PendSV Handler
                DCD     SysTick_Handler            ; SysTick Handler

                
                ; External Interrupts                                            
                DCD     Default_IRQ_Handler        ; 16: UART1                   
                DCD     DMA_LISR                   ; 17: DMA                     
                DCD     Default_IRQ_Handler        ; 18: HIF                     
                DCD     hal_i2c_callback           ; 19: I2C1                    
                DCD     hal_i2c_callback           ; 20: I2C2                    
                DCD     Default_IRQ_Handler        ; 21: UART2                   
                DCD     Default_IRQ_Handler        ; 22: MTK_CRYPTO              
                DCD     Default_IRQ_Handler        ; 23: SF                      
                DCD     Default_IRQ_Handler        ; 24: EINT                    
                DCD     Default_IRQ_Handler        ; 25: BTIF                    
                DCD     hal_wdt_isr                ; 26: WDT                     
                DCD     Default_IRQ_Handler        ; 27: reserved                
                DCD     Default_IRQ_Handler        ; 28: SPI_SLAVE               
                DCD     Default_IRQ_Handler        ; 29: WDT_N9                  
                DCD     halADC_LISR                ; 30: ADC                     
                DCD     Default_IRQ_Handler        ; 31: IRDA_TX                 
                DCD     Default_IRQ_Handler        ; 32: IRDA_RX                 
                DCD     Default_IRQ_Handler        ; 33: USB_VBUS_ON             
                DCD     Default_IRQ_Handler        ; 34: USB_VBUS_OFF            
                DCD     Default_IRQ_Handler        ; 35: timer_hit               
                DCD     Default_IRQ_Handler        ; 36: GPT3                    
                DCD     Default_IRQ_Handler        ; 37: alarm_hit               
                DCD     Default_IRQ_Handler        ; 38: AUDIO                   
                DCD     Default_IRQ_Handler        ; 39: n9_cm4_sw_irq           
                DCD     GPT_INT_Handler            ; 40: GPT4                    
                DCD     halADC_COMP_LISR           ; 41: adc_comp_irq            
                DCD     Default_IRQ_Handler        ; 42: reserved                
                DCD     Default_IRQ_Handler        ; 43: SPIM                    
                DCD     Default_IRQ_Handler        ; 44: USB                     
                DCD     Default_IRQ_Handler        ; 45: UDMA                    
                DCD     Default_IRQ_Handler        ; 46: TRNG                    
                DCD     Default_IRQ_Handler        ; 47: reserved                
                DCD     Default_IRQ_Handler        ; 48: configurable            
                DCD     Default_IRQ_Handler        ; 49: configurable            
                DCD     Default_IRQ_Handler        ; 50: configurable            
                DCD     Default_IRQ_Handler        ; 51: configurable            
                DCD     Default_IRQ_Handler        ; 52: configurable            
                DCD     Default_IRQ_Handler        ; 53: configurable            
                DCD     Default_IRQ_Handler        ; 54: configurable            
                DCD     Default_IRQ_Handler        ; 55: configurable            
                DCD     Default_IRQ_Handler        ; 56: configurable            
                DCD     Default_IRQ_Handler        ; 57: configurable            
                DCD     Default_IRQ_Handler        ; 58: configurable            
                DCD     Default_IRQ_Handler        ; 59: configurable            
                DCD     Default_IRQ_Handler        ; 60: configurable            
                DCD     Default_IRQ_Handler        ; 61: configurable            
                DCD     Default_IRQ_Handler        ; 62: configurable            
                DCD     Default_IRQ_Handler        ; 63: configurable            
                DCD     Default_IRQ_Handler        ; 64: configurable            
                DCD     Default_IRQ_Handler        ; 65: configurable            
                DCD     Default_IRQ_Handler        ; 66: configurable            
                DCD     Default_IRQ_Handler        ; 67: configurable            
                DCD     Default_IRQ_Handler        ; 68: configurable            
                DCD     Default_IRQ_Handler        ; 69: configurable            
                DCD     Default_IRQ_Handler        ; 70: configurable            
                DCD     Default_IRQ_Handler        ; 71: configurable            
                DCD     Default_IRQ_Handler        ; 72: configurable            
                DCD     Default_IRQ_Handler        ; 73: configurable            
                DCD     Default_IRQ_Handler        ; 74: configurable            
                DCD     Default_IRQ_Handler        ; 75: configurable            
                DCD     Default_IRQ_Handler        ; 76: configurable            
                DCD     Default_IRQ_Handler        ; 77: configurable            
                DCD     Default_IRQ_Handler        ; 78: configurable            
                DCD     Default_IRQ_Handler        ; 79: configurable            
                DCD     Default_IRQ_Handler        ; 80: configurable            
                DCD     Default_IRQ_Handler        ; 81: configurable            
                DCD     Default_IRQ_Handler        ; 82: configurable            
                DCD     Default_IRQ_Handler        ; 83: configurable            
                DCD     Default_IRQ_Handler        ; 84: configurable            
                DCD     Default_IRQ_Handler        ; 85: configurable            
                DCD     Default_IRQ_Handler        ; 86: configurable            
                DCD     Default_IRQ_Handler        ; 87: configurable            
                DCD     Default_IRQ_Handler        ; 88: configurable            
                DCD     Default_IRQ_Handler        ; 89: configurable            
                DCD     Default_IRQ_Handler        ; 90: configurable            
                DCD     Default_IRQ_Handler        ; 91: configurable            
                DCD     Default_IRQ_Handler        ; 92: configurable            
                DCD     Default_IRQ_Handler        ; 93: configurable            
                DCD     Default_IRQ_Handler        ; 94: configurable            
                DCD     Default_IRQ_Handler        ; 95: configurable            
                DCD     Default_IRQ_Handler        ; 96: configurable            
                DCD     Default_IRQ_Handler        ; 97: configurable            
                DCD     Default_IRQ_Handler        ; 98: configurable            
                DCD     Default_IRQ_Handler        ; 99: configurable            
                DCD     Default_IRQ_Handler        ; 100: configurabl            
                DCD     Default_IRQ_Handler        ; 101: configurabl            
                DCD     Default_IRQ_Handler        ; 102: configurabl            
                DCD     Default_IRQ_Handler        ; 103: configurabl            
                DCD     Default_IRQ_Handler        ; 104: configurabl            
                DCD     Default_IRQ_Handler        ; 105: configurabl            
                DCD     Default_IRQ_Handler        ; 106: configurabl            
                DCD     Default_IRQ_Handler        ; 107: configurabl            
                DCD     Default_IRQ_Handler        ; 108: configurabl            
                DCD     Default_IRQ_Handler        ; 109: configurabl            
                DCD     Default_IRQ_Handler        ; 110: configurabl            
                DCD     Default_IRQ_Handler        ; 111: configurabl 

__Vectors_End

__Vectors_Size  EQU  __Vectors_End - __isr_vector 

                AREA    |.reset_handler|, CODE, READONLY


Reset_Handler   PROC
                EXPORT  Reset_Handler              [WEAK]
                IMPORT  SystemInit
                IMPORT  __main

                LDR     SP, =__initial_sp

                LDR     R0, =SystemInit
                BLX     R0

                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler                [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler          [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler          [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler           [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler         [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler                [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler           [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler             [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler            [WEAK]
                B       .
                ENDP

Default_IRQ_Handler  PROC
                EXPORT  Default_IRQ_Handler        [WEAK]
                B       .
                ENDP


Default_Handler PROC
                
                EXPORT    DMA_LISR              [WEAK]   
                EXPORT    hal_i2c_callback      [WEAK]   
                EXPORT    hal_wdt_isr           [WEAK]   
                EXPORT    halADC_LISR           [WEAK]   
                EXPORT    GPT_INT_Handler       [WEAK]   
                EXPORT    halADC_COMP_LISR      [WEAK]           

DMA_LISR          
hal_i2c_callback  
hal_wdt_isr       
halADC_LISR       
GPT_INT_Handler   
halADC_COMP_LISR  

                B       .

                ENDP

                ALIGN

;*******************************************************************************
; User Stack and Heap initialization
;*******************************************************************************
                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

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
