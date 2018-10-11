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
 * @author  Mediatek
 * @version 1.0
 * @date    2015.11.11
 *
 */

#ifndef __HAL_CM4_TOPSM_H__
#define __HAL_CM4_TOPSM_H__

#include "hal_platform.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED

#include "hal_sleep_manager_platform.h"
#include "memory_attribute.h"

/** PSI Master Index */
#define SLEEP_PSI_MASTER_SW 0x0
#define SLEEP_PSI_MASTER_CM4 0x1
#define SLEEP_PSI_MASTER_BT 0x2
#define SLEEP_PSI_MASTER_RESERVE 0x3
#define SLEEP_PSI_MASTER_Dsp 0x4

#define SLEEP_PSI_MASTER_CM4 0x1
#define CMCFG_SYSRAM_CFG (volatile uint32_t*)0xA211000C

#define TIMER_NORMAL_STATE      0001
#define TIMER_PAUSE_STATE       0010
#define TIMER_PRE_PAUSE_STATE   0100
#define TIMER_SETTLE_STATE      1000
#define TIMER_PAUSE_END_STATE   1111
#define CLOCK_SOURCE_32K_FREQ   32.768
#define PLL_DPM_CON1                            ((volatile uint32_t*) 0xA2040094)
#define CM4_TOPSM_MODEM_CG_CLR2                 ((volatile uint32_t*) 0x83000018)  /**/

//Fow Low Power Setting
#define GPIO_TDSEL0_SET                         ((volatile uint32_t*)0xA2020D04)
#define GPIO_TDSEL1_SET                         ((volatile uint32_t*)0xA2020D14)
#define GPIO_TDSEL2_SET                         ((volatile uint32_t*)0xA2020D24)
#define GPIO_TDSEL3_SET                         ((volatile uint32_t*)0xA2020D34)
#define GPIO_TDSEL0_CLR                         ((volatile uint32_t*)0xA2020D08)
#define GPIO_TDSEL1_CLR                         ((volatile uint32_t*)0xA2020D18)
#define GPIO_TDSEL2_CLR                         ((volatile uint32_t*)0xA2020D28)
#define GPIO_TDSEL3_CLR                         ((volatile uint32_t*)0xA2020D38)

//#define CM4_TOPSM_BASE                                                           (0xA2100000) /* CM4_TOPSM_BASE: 0x83010000 is used for Larkspur */
#define CM4_TOPSM_RM_CLK_SETTLE                 ((volatile uint32_t*)(CM4_TOPSM_BASE+0x00))  /* Resource Manager Clock Settling Setting */
#define CM4_TOPSM_RM_TMR_TRG0                   ((volatile uint32_t*)(CM4_TOPSM_BASE+0x10))  /* Resource Manager Timer Cross Trigger Control 0 */
#define CM4_TOPSM_RM_TMR_PWR0                   ((volatile uint32_t*)(CM4_TOPSM_BASE+0x18))  /* Resource Manager Timer's Power Power Enable Control (only when set HW mode) */
#define CM4_TOPSM_RM_PWR_PER0                   ((volatile uint32_t*)(CM4_TOPSM_BASE+0x824))
#define CM4_TOPSM_RM_PWR_PER1                   ((volatile uint32_t*)(CM4_TOPSM_BASE+0x828))
#define CM4_TOPSM_RM_PWR_CON0                   ((volatile uint32_t*)(CM4_TOPSM_BASE+0x800))  /* Power Control Register0 Specify MTCMOS SUBSYS Power Control Sequence,
                                                                                       and it is controlled by Software mode or hardware mode. Other status or control option
                                                                                        MD2G SUBSYS Power Control Register (SW or HW mode)  */
#define CM4_TOPSM_PROTECT_ACK_MASK              ((volatile uint32_t*)(CM4_TOPSM_BASE+0xA38))  /* Bus Protection Ack Mask */
#define CM4_TOPSM_CCF_CLK_CON                   ((volatile uint16_t*)(CM4_TOPSM_BASE+0x200))  /* Clock overrides (Powerful debug usage) */
#define CM4_TOPSM_SM_TRIG_SETTLE0               ((volatile uint32_t*)(CM4_TOPSM_BASE+0x900))  /* Sleep Manager Trigger Settle Time 0 Topsm settling time.
                                                                                           * Note: When clock resource of topsm is not ready, topsm Settling time is  */
#define CM4_TOPSM_SM_REQ_MASK                   ((volatile uint32_t*)(CM4_TOPSM_BASE+0x8B0))  /* Sleep Manager Request Source Mask
Mask for software wakeup and timer based wakeup request to other sleep manager*/
#define CM4_TOPSM_RM_SM_TRG                     ((volatile uint32_t*)(CM4_TOPSM_BASE+0x860))  /* Sleep Manager Cross Trigger Control */
#define CM4_TOPSM_RM_SM_PWR                     ((volatile uint32_t*)(CM4_TOPSM_BASE+0x850))  /* Sleep Manager Power Enable Control */
#define CM4_TOPSM_RM_SM_PLL_MASK0               ((volatile uint32_t*)(CM4_TOPSM_BASE+0x840))  /* Sleep Manager PLL Enable Control */
#define CM4_TOPSM_INDIV_CLK_PROTECT_ACK_MASK    ((volatile uint32_t*)(CM4_TOPSM_BASE+0xA34))  /* Clock Bus Protection Ack Mask */
#define CM4_TOPSM_RM_TMR_SSTA                   ((volatile uint32_t*)(CM4_TOPSM_BASE+0x40))  /* Resource Manager Timer Sleep Control State */
#define CM4_TOPSM_SM_SLV_REQ_IRQ                ((volatile uint32_t*)(CM4_TOPSM_BASE+0x8C0))  /* Sleep Manager Request Interrupt */
#define CM4_TOPSM_SM_MAS_RDY_IRQ                ((volatile uint32_t*)(CM4_TOPSM_BASE+0x8D0))  /* Sleep Manager Ready Interrupt */
#define CM4_TOPSM_FRC_CON                       ((volatile uint32_t*)(CM4_TOPSM_BASE+0x80))  /* Free Running Counter Control Register (with Magic Word: 0x6276xxxx) */
#define CM4_TOPSM_SM_SLV_REQ_STA                ((volatile uint32_t*)(CM4_TOPSM_BASE+0x8E0)) /*Sleep Manager Request Status*/
#define CM4_TOPSM_DBG_RM_SM_MASK                ((volatile uint32_t*)(CM4_TOPSM_BASE+0x880))    /*Sleep Manager Debug Control Mask*/
#define CM4_TOPSM_SW_CLK_FORCE_ON_SET           ((volatile uint32_t*)(CM4_TOPSM_BASE+0xA04))  /*Software Clock Force On Set*/
#define CM4_TOPSM_MCF_CNT_BASE                  ((volatile uint32_t*)(CM4_TOPSM_BASE+0xA30))  /*   MTCMOS Counter Base   */

#define CM4_TOPSM_FRC_VAL_R                     ((volatile uint32_t*)(CM4_TOPSM_BASE+0x88))  /* Free Running Counter Current Value */
#define CM4_TOPSM_FRC_F32K_FM                   ((volatile uint32_t*)(CM4_TOPSM_BASE+0x84))  /* Free Running Counter Current Value */
#define CM4_TOPSM_RM_PWR_STA                    ((volatile uint32_t*)(CM4_TOPSM_BASE+0x820))
#define CM4_TOPSM_F32K_CNT                      ((volatile uint32_t*)(CM4_TOPSM_BASE+0x104))
#define CM4_TOPSM_TOPSM_DBG                      ((volatile uint32_t*)(CM4_TOPSM_BASE+0x50))
#define CM4_TOPSM_SYSTEM_CONTROL                ((volatile uint32_t*)0xE000ED10)
#define CM4_TOPSM_MODEM_MEM_CON                 ((volatile uint32_t*)0x83000048)
#define CM4_TOPSM_SCR                           ((volatile uint32_t*)0xE000ED10)
#define CMCFG_BOOT_VECTOR1                      ((volatile uint32_t*)0xA2110004)
#define CMCFG_BOOT_FROM_SLV                     ((volatile uint32_t*)0xA2110008)
//For Low Power Setting
#define PMIC_WR_PATH_CON0                       ((volatile uint32_t*)0xA21A0000)
#define ABBA_AUDIODL_CON4_W                       ((volatile uint32_t*)0xA21C0090)

#define CM4_TOPSM_SYSCLK_SETTLE 0X2
#define CM4_TOPSM_PLL_SETTLE    0X1E

#define CM4_TOPSM_INT_MASK           ((volatile uint16_t*)(CM4_OSTIMER_BASE+0x30))  /* Interrupt Mask (All default masked: disabled)       */
#define CM4_TOPSM_DBG_WAKEUP         ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x60))  /* Debug Wakeup        */
#define CM4_TOPSM_FRM_F32K           ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x10))  /* Frame Duration by 32K clock [8:0]  */
#define CM4_TOPSM_CMD                ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x04))  /* Command Register*/
#define CM4_TOPSM_STA                ((volatile uint16_t*)(CM4_OSTIMER_BASE+0x08))  /* Command Status Register*/
#define CM4_TOPSM_EVENT_MASK         ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x50))  /* Event Mask (All default enabled)*/
#define CM4_TOPSM_WAKEUP_STA         ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x54))  /* Event Wakeup Status [auto-clear with source]: record the first wakeup sources       */
#define CM4_TOPSM_ISR                ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x40))  /* Interrupt Status        */
#define CM4_TOPSM_FRM                ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x0C))  /* Frame Duration [12:0]        */
#define CM4_TOPSM_CON                ((volatile uint16_t*)(CM4_OSTIMER_BASE+0x00))  /* Control Register  0: EN*/
#define CM4_TOPSM_UFN_R              ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x20))  /* Current OS Timer Un-alignement Frame Number [31:0] */
#define CM4_TOPSM_AFN_R              ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x24))  /* Current OS Timer Alignement Frame Number [31:0] */
#define CM4_TOPSM_AFN_DLY            ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x1C))  /* OS Timer Alignment Frame Delay Number [31:0]*/
#define CM4_TOPSM_UFN                ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x14))  /* OS Timer Un-alignment Frame Number [31:0]  */
#define CM4_TOPSM_AFN                ((volatile uint32_t*)(CM4_OSTIMER_BASE+0x18))  /* OS Timer Alignment Frame Number [31:0] */
typedef void (*cm4_topsm_cb)(void *data);

typedef enum {
    MTCMOS_BT_OFF,
    MTCMOS_BT_ON,
    MTCMOS_MM_OFF,
    MTCMOS_MM_ON,
    MTCMOS_INFRA_OFF,
    MTCMOS_INFRA_ON,
    MTCMOS_DSP_OFF,
    MTCMOS_DSP_ON,
} hal_cm4_topsm_sleep_periphera_control_t;

typedef enum {
    CM4_TOPSM_FALSE = 0, /*CM4 TOPSM read back AFN/UFN and found that current value is <=2*/
    CM4_TOPSM_SUCCESS_TRUE, /*CM4 TOPSM has successfully set the AFN value into ARM OS Timer HW*/
    CM4_TOPSM_TIME_OUT, /*CM4 TOPSM failed to poll ready bit*/
    CM4_TOPSM_FAIL, /* Other unknown fail reason */
    CM4_TOPSM_RESULT_NUM
} CM4_TOPSM_RESULT_Enum;

typedef struct {
    int   MM_Count;
    int   BT_Count;
    int   MD2G_Count;
} cm4_topsm_mtcmos_control;

typedef struct {
    volatile uint32_t PSR;      /* ASOR,IRSR,EPSR */
    volatile uint32_t PRIMASK;
    volatile uint32_t FAULTMASK;
    volatile uint32_t BASEPRI;
    volatile uint32_t CONTROL;
} CPU_CORE_BAKEUP_REG_T;

typedef struct hal_cm4_topsm_cb_t {
    cm4_topsm_cb func;
    void *para;
} hal_cm4_topsm_cb_t;

int cm4_topsm_lock_BT_MTCMOS(void);
int cm4_topsm_lock_MM_MTCMOS(void);
int cm4_topsm_lock_MD2G_MTCMOS(void);
int cm4_topsm_unlock_BT_MTCMOS(void);
int cm4_topsm_unlock_MM_MTCMOS(void);
int cm4_topsm_unlock_MD2G_MTCMOS(void);
void cm4_tdma_enter_sleep(void);
void cm4_topsm_sleep_default_callback_function(void);
void cm4_topsm_set_sleep_timeframe(uint32_t sleep_time, uint32_t frame, uint32_t duration);
void cm4_topsm_enter_suspend(hal_sleep_mode_t mode);
void cm4_topsm_rm_init_interrupt(void);
void cm4_topsm_rm_init_misc(void);
void cm4_topsm_enter_deep_sleep(hal_sleep_mode_t mode);
void cm4_topsm_init(void);
uint32_t cm4_topsm_get_free_run_counter(void);
uint32_t cm4_topsm_get_32k_free_run_counter(void);
bool hal_cm4_topsm_register_suspend_cb(cm4_topsm_cb func, void *data);
bool hal_cm4_topsm_register_resume_cb(cm4_topsm_cb func, void *data);
bool cm4_topsm_wakeup_set_one_irq_enable(hal_sleep_manager_wakeup_source_t index);
bool cm4_topsm_wakeup_set_one_irq_disable(hal_sleep_manager_wakeup_source_t index);
void cm4_topsm_wakeup_mask_all_irq(void);
void cm4_topsm_wakeup_unmask_all_irq(void);
void cm4_topsm_debug_option(bool sta);

void cm4_topsm_debug_dump_wakeup_source(void);
void cm4_topsm_debug_dump_backup_restore_time(void);
#if (defined (__GNUC__) || defined (__ICCARM__))    /* GCC,IAR compiler */

#define __CPU_CORE_REG_BACKUP(ptr) __asm volatile(  \
    "PUSH   {r0-r12, lr}                \n"\
    "MOV    r2,         %0              \n"\
    "MOV    r0,         r2              \n"\
    "MRS    r1,         psr             \n"\
    "STR    r1,         [r0],#4         \n"\
    "MRS    r1,         primask         \n"\
    "STR    r1,         [r0],#4         \n"\
    "MRS    r1,         faultmask       \n"\
    "STR    r1,         [r0],#4         \n"\
    "MRS    r1,         basepri         \n"\
    "STR    r1,         [r0],#4         \n"\
    "MRS    r1,         control         \n"\
    "STR    r1,         [r0]            \n"\
    "POP    {r0-r12,lr}                   "\
    : :"r" (ptr):                          \
  );

#define __CPU_STACK_POINT_BACKUP(psp,msp) {     \
    __asm volatile("MOV %0, sp" : "=r"(psp));   \
    __asm volatile("MOV r0, #0");               \
    __asm volatile("MSR control, r0");          \
    __asm volatile("MSR control, r0");          \
    __asm volatile("MOV %0, sp" : "=r"(msp));   \
}

#define __MSP_RESTORE(ptr) __asm volatile(  \
    "MOV    r2,         %0              \n"\
    "MOV    r0,         r2              \n"\
    "LDR    r0,         [r0]            \n"\
    "MSR    msp,        r0                "\
    : :"r"(ptr)                            \
  );

#define __SWITCH_TO_PSP_STACK_POINT() __asm volatile(  \
    "MOV    r0,         #4              \n"\
    "MSR    control,    r0              \n"\
    "DSB                                \n"\
    "MOV    r0,         #6              \n"\
    "MSR    control,    r0              \n"\
    "DSB                                  "\
  );

#define __PSP_RESTORE(ptr) __asm volatile(  \
    "MOV    r2,         %0              \n"\
    "MOV    r0,         r2              \n"\
    "LDR    r0,         [r0]            \n"\
    "MSR    psp,        r0                "\
    : :"r"(ptr)                            \
  );

#define __CPU_CORE_CONTROL_REG_RESTORE(ptr) __asm volatile(  \
    "MOV    r2,         %0              \n"\
    "MOV    r0,         r2              \n"\
    "LDR    r1,         [r0],#0         \n"\
    "MSR    control,    r1              \n"\
    : :"r" (ptr):                          \
  );

#define __CPU_CORE_REG_RESTORE(ptr) __asm volatile(  \
    "PUSH   {r0-r12, lr}                \n"\
    "MOV    r2,         %0              \n"\
    "MOV    r0,         r2              \n"\
    "LDR    r1, [r0],   #4              \n"\
    "MSR    psr,        r1              \n"\
    "LDR    r1, [r0],   #4              \n"\
    "MSR    primask,    r1              \n"\
    "LDR    r1, [r0],   #4              \n"\
    "MSR    faultmask,  r1              \n"\
    "LDR    r1, [r0],   #4              \n"\
    "MSR    basepri,    r1              \n"\
    "POP    {r0-r12,lr}                   "\
    : :"r" (ptr):                          \
  );

#define __ENTER_DEEP_SLEEP(ptr) __asm volatile(  \
    "MOV    r3,         %0              \n"\
    "MOV    r2,         pc              \n"\
    "ADD    r2,         r2,#16          \n"\
    "ORR    r2,         r2,#1           \n"\
    "STR    r2,         [r3]            \n"\
    "DSB                                \n"\
    "WFI                                \n"\
    "NOP                                \n"\
    "ISB                                \n"\
    "CPSID I                            \n"\
    : :"r"(ptr)                            \
  );

#define __POP_CPU_REG() {     \
    __asm volatile("POP    {r0-r12,lr}");   \
}

#elif defined (__CC_ARM)    /* MDK compiler */

#define __CPU_CORE_REG_BACKUP(ptr) __asm volatile( \
    "MOV    r2,         "#ptr"          \n"\
    "MOV    r0,         r2              \n"\
    "MRS    r1,         psr             \n"\
    "STR    r1,         [r0],#4         \n"\
    "MRS    r1,         primask         \n"\
    "STR    r1,         [r0],#4         \n"\
    "MRS    r1,         faultmask       \n"\
    "STR    r1,         [r0],#4         \n"\
    "MRS    r1,         basepri         \n"\
    "STR    r1,         [r0],#4         \n"\
    "MRS    r1,         control         \n"\
    "STR    r1,         [r0]            \n"\
  )

#define __CPU_CORE_REG_RESTORE(ptr) __asm volatile( \
    "MOV    r2,         "#ptr"          \n"\
    "MOV    r0,         r2              \n"\
    "LDR    r1,         [r0],#4         \n"\
    "MSR    psr,        r1              \n"\
    "LDR    r1,         [r0],#4         \n"\
    "MSR    primask,    r1              \n"\
    "LDR    r1,[r0],    #4              \n"\
    "MSR    faultmask,  r1              \n"\
    "LDR    r1,[r0],    #4              \n"\
    "MSR    basepri,    r1              \n"\
)

#define __CPU_STACK_POINT_BACKUP(psp,msp) {         \
    __asm volatile("MOV "#psp", __current_sp()");   \
    __asm volatile("MOV r0, #0");                   \
    __asm volatile("MSR control, r0");              \
    __asm volatile("MSR control, r0");              \
    __asm volatile("MOV "#msp", __current_sp()");   \
}

#define __MSP_RESTORE(ptr) __asm volatile( \
    "MOV    r2,         "#ptr"          \n"\
    "MOV    r0,         r2              \n"\
    "LDR    r0,         [r0]            \n"\
    "MSR    msp,        r0              \n"\
  )

#define __CPU_CORE_CONTROL_REG_RESTORE(ptr) __asm volatile( \
    "MOV r2, "#ptr" \n"\
    "MOV r0, r2 \n"\
    "LDR r1, [r0],#0 \n"\
    "MSR control, r1 \n"\
)

#define __SWITCH_TO_PSP_STACK_POINT() __asm volatile( \
    "MOV    r0,         #4              \n"\
    "MSR    control,    r0              \n"\
    "DSB                                \n"\
    "MOV    r0,         #6              \n"\
    "MSR    control,    r0              \n"\
    "DSB                                \n"\
  );

#define __PSP_RESTORE(ptr) __asm volatile(  \
    "MOV    r2,         "#ptr"          \n"\
    "MOV    r0,         r2              \n"\
    "LDR    r0,         [r0]            \n"\
    "MSR    psp,        r0              \n"\
  );

#define __ENTER_DEEP_SLEEP(ptr) __asm volatile( \
    "MOV    r3,         "#ptr"          \n"\
    "MOV    r2,         __current_pc()  \n"\
    "ADD    r2,         r2,#16          \n"\
    "ORR    r2,         r2,#1           \n"\
    "STR    r2,         [r3]            \n"\
    "DSB                                \n"\
    "WFI                                \n"\
    "NOP                                \n"\
    "ISB                                \n"\
    "CPSID I                            \n"\
)

#define __BACKUP_SP(ptr) __asm volatile("MOV "#ptr", __current_sp()\n")

ATTR_TEXT_IN_TCM static __inline __asm volatile void __PUSH_CPU_REG(void)
{
    PRESERVE8
    PUSH    {r0 - r12}
    BX      r14
}

ATTR_TEXT_IN_TCM static __inline __asm volatile void __POP_CPU_REG(void)
{
    PRESERVE8
    POP     {r0 - r12}
    BX      r14
}

ATTR_TEXT_IN_TCM static __inline __asm volatile void __RESTORE_LR(unsigned int return_address)
{
    PRESERVE8
    MOV     lr, r0
    BX      r14
}
#endif  /* MDK compiler */

#define INFRA_POWER_OFF {                                       \
    *SYS_TOPSM_RM_TMR_PWR0  = 0x0;                              \
    *SYS_TOPSM_RM_SM_PWR    = 0x0;                              \
    *SYS_TOPSM_RM_PWR_CON3  = 0x0004223A;   /*SW mode prepare*/ \
    hal_gpt_delay_us(1);                                        \
    *SYS_TOPSM_RM_PWR_CON3  = 0x000422BB;   /*SW mode*/         \
    while ((*SYS_TOPSM_RM_PWR_STA & 0x80000) == 0x80000);       \
    *SYS_TOPSM_RM_PWR_STA   = 0x8;                              \
    *SYS_TOPSM_RM_SM_MASK   = 0x0;                              \
    *SYS_TOPSM_RM_SM_PWR    = 0x3000;                           \
}

#define INFRA_POWER_ON {                                        \
    *SYS_TOPSM_RM_PWR_CON3 = 0x000442BE;    /*pwr_on_0 up*/     \
    hal_gpt_delay_us(1);                                        \
    *SYS_TOPSM_RM_PWR_CON3 = 0x000642BE;    /*pwr_on_1 up*/     \
    hal_gpt_delay_us(1);                                        \
    *SYS_TOPSM_RM_PWR_CON3 = 0x000642B6;    /*pwr_mem_off down*/\
    hal_gpt_delay_us(1);                                        \
    *SYS_TOPSM_RM_PWR_CON3 = 0x000E42B6;    /*mem_isointb down*/\
    hal_gpt_delay_us(1);                                        \
    *SYS_TOPSM_RM_PWR_CON3 = 0x000E42A6;    /*clk_dis down*/    \
    hal_gpt_delay_us(1);                                        \
    *SYS_TOPSM_RM_PWR_CON3 = 0x000E42A4;    /*pwr_iso down*/    \
    hal_gpt_delay_us(1);                                        \
    *SYS_TOPSM_RM_PWR_CON3 = 0x000E42B4;    /*clk_dis up*/      \
    hal_gpt_delay_us(1);                                        \
    *SYS_TOPSM_RM_PWR_CON3 = 0x000E4295;    /*mem_prot down & pwr_rst_b up*/\
    hal_gpt_delay_us(1);                                        \
    *SYS_TOPSM_RM_PWR_CON3 = 0x000E4285;    /*clk_dis down*/    \
    hal_gpt_delay_us(1);                                        \
    *SYS_TOPSM_RM_PWR_CON3 = *SYS_TOPSM_RM_PWR_CON3 | 0x00000044;\
    while (((*SYS_TOPSM_RM_PWR_STA & 0x8000000) == 0) || ((*SYS_TOPSM_RM_PWR_STA & 0x80000) == 0));\
}

#define GPIO_TDSEL_SET {            \
    *GPIO_TDSEL0_SET = 0xFFFFFFFF;  \
    *GPIO_TDSEL1_SET = 0xFFFFFFFF;  \
    *GPIO_TDSEL2_SET = 0xFFFFFFFF;  \
    *GPIO_TDSEL3_SET = 0xFFFFFFFF;  \
}

#define GPIO_TDSEL_CLR {            \
    *GPIO_TDSEL0_CLR = 0xFFFFFFFF;  \
    *GPIO_TDSEL1_CLR = 0xFFFFFFFF;  \
    *GPIO_TDSEL2_CLR = 0xFFFFFFFF;  \
    *GPIO_TDSEL3_CLR = 0xFFFFFFFF;  \
}

#endif /* HAL_SLEEP_MANAGER_ENABLED */
#endif /* __HAL_CM4_TOPSM_H__ */

