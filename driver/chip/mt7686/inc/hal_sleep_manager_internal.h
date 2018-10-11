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

#ifndef __HAL_SLEEP_MANAGER_INTERNAL_H__
#define __HAL_SLEEP_MANAGER_INTERNAL_H__
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#include "hal_sleep_manager_platform.h"
#include "hal_spm.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "memory_attribute.h"
#include "core_cm4.h"
#include "memory_attribute.h"

//#define SLEEP_MANAGEMENT_DEBUG_ENABLE
//#define SLEEP_MANAGEMENT_DEBUG_SLEEP_WAKEUP_LOG_ENABLE

#define CM4_SYSTEM_CONTROL          ((volatile uint32_t*)0xE000ED10)
#define CMCFG_BOOT_VECTOR0          ((volatile uint32_t*)0xA21E0000)
#define CMCFG_BOOT_VECTOR1          (0xA21E0004)
#define CMCFG_BOOT_FROM_SLV         ((volatile uint32_t*)0xA21E0008)

ATTR_TEXT_IN_TCM void sleep_management_enter_deep_sleep(hal_sleep_mode_t mode);

typedef struct {
    volatile uint32_t PSR;      /* ASOR,IRSR,EPSR */
    volatile uint32_t PRIMASK;
    volatile uint32_t FAULTMASK;
    volatile uint32_t BASEPRI;
    volatile uint32_t CONTROL;
} CPU_CORE_BAKEUP_REG_T;

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
    "CPSID I                            \n"\
    "ISB                                  "\
    : :"r"(ptr)                            \
  );

#define __POP_CPU_REG() __asm volatile(  \
    "POP    {r0-r12,lr}                 "\
  );

#elif defined (__CC_ARM)    /* MDK compiler */

#define __CPU_CORE_REG_BACKUP(ptr) __asm volatile( \
    "MOV 	r2,         "#ptr"          \n"\
    "MOV 	r0,         r2              \n"\
    "MRS 	r1,         psr             \n"\
    "STR 	r1,         [r0],#4         \n"\
    "MRS 	r1,         primask         \n"\
    "STR 	r1,         [r0],#4         \n"\
    "MRS 	r1,         faultmask       \n"\
    "STR 	r1,         [r0],#4         \n"\
    "MRS 	r1,         basepri         \n"\
    "STR 	r1,         [r0],#4         \n"\
    "MRS 	r1,         control         \n"\
    "STR 	r1,         [r0]            \n"\
  )

#define __CPU_CORE_REG_RESTORE(ptr) __asm volatile( \
    "MOV 	r2,         "#ptr"          \n"\
    "MOV 	r0,         r2              \n"\
    "LDR 	r1,         [r0],#4 		\n"\
    "MSR 	psr,        r1              \n"\
    "LDR 	r1,         [r0],#4 		\n"\
    "MSR 	primask,    r1              \n"\
    "LDR 	r1,[r0],    #4              \n"\
    "MSR 	faultmask,  r1              \n"\
    "LDR 	r1,[r0],    #4              \n"\
    "MSR 	basepri,    r1              \n"\
)

#define __CPU_STACK_POINT_BACKUP(psp,msp) {     	\
    __asm volatile("MOV "#psp", __current_sp()");	\
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
    "MOV 	r3,         "#ptr"          \n"\
    "MOV 	r2,         __current_pc()  \n"\
    "ADD 	r2,         r2,#16          \n"\
    "ORR 	r2,         r2,#1           \n"\
    "STR 	r2,         [r3]            \n"\
    "DSB                                \n"\
    "WFI                                \n"\
    "NOP                                \n"\
    "CPSID I                            \n"\
    "ISB                                \n"\
)

#define __BACKUP_SP(ptr) __asm volatile("MOV "#ptr", __current_sp()\n")

ATTR_TEXT_IN_TCM static __inline __asm volatile void __PUSH_CPU_REG(void)
{
    PRESERVE8
    PUSH    {r0 - r12}
    BX 		r14
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

#define SAVE_PRIORITY_GROUP (IRQ_NUMBER_MAX)
typedef struct {
    uint32_t nvic_iser;                    /**< eint hardware debounce time */
    uint32_t nvic_iser1;
    uint32_t nvic_ip[SAVE_PRIORITY_GROUP];
} nvic_sleep_backup_register_t;

typedef struct {
    volatile uint32_t ACTLR;    /* Auxiliary Control Register */
    volatile uint32_t VTOR;     /* Vector Table Offset Register */
    volatile uint32_t SCR;      /* System Control Register */
    volatile uint32_t CCR;      /* Configuration Control Register */
    volatile uint8_t SHP[12];   /* System Handlers Priority Registers (4-7, 8-11, 12-15) */
    volatile uint32_t SHCSR;    /* System Handler Control and State Register */
    volatile uint32_t CPACR;    /* Coprocessor Access Control Register */
} CM4_SYS_CTRL_BAKEUP_REG_T;

/* FPU backup register struct */
typedef struct {
    volatile uint32_t FPCCR;
    volatile uint32_t FPCAR;
} FPU_BAKEUP_REG_T;

/* CMSYS_CFG backup register struct */
typedef struct {
    volatile uint32_t STCALIB;
    volatile uint32_t AHB_BUFFERALBE;
    volatile uint32_t AHB_FIFO_TH;
    volatile uint32_t INT_ACTIVE_HL0;
    volatile uint32_t INT_ACTIVE_HL1;
    volatile uint32_t DCM_CTRL_REG;
} CMSYS_CFG_BAKEUP_REG_T;

/* CMSYS_CFG_EXT backup register struct */
typedef struct {
    __IO uint32_t CG_EN;
    __IO uint32_t DCM_EN;
} CMSYS_CFG_EXT_BAKEUP_REG_T;

typedef enum {
    UNLOCK_SLEEP    = 0,
    LOCK_SLEEP      = 1
} sleep_management_lock_sleep_t;

typedef enum {
    SLEEP_MANAGEMENT_UNINITIALIZED  = 0,
    SLEEP_MANAGEMENT_INITIALIZED    = 1
} sleep_management_init_status_t;

/* Lock Sleep Handle ID : 0~31 */
#define  SLEEP_LOCK_HANDLE_MAX          32
#define  SLEEP_LOCK_HANDLE_USER_MAX     (SLEEP_LOCK_HANDLE_MAX-SLEEP_LOCK_USER_START_ID)
#define  SLEEP_HANDLE_NAME_LEN          11

typedef struct {
    uint32_t    lock_sleep_request;
    uint32_t    user_handle_resoure;
    uint8_t     user_handle_count;
    uint8_t     lock_sleep_request_count[SLEEP_LOCK_HANDLE_MAX];
    uint8_t     user_handle_name[SLEEP_LOCK_HANDLE_USER_MAX][SLEEP_HANDLE_NAME_LEN];
} sleep_management_handle_t;

typedef void(* sleep_management_suspend_callback_t)(void *data);
typedef void(* sleep_management_resume_callback_t)(void *data);

typedef struct {
    sleep_management_suspend_callback_t func;
    void *data;
    bool init_status;
} sleep_management_suspend_callback_func_t;

typedef struct {
    sleep_management_resume_callback_t func;
    void *data;
    bool init_status;
} sleep_management_resume_callback_func_t;

typedef enum {
    SLEEP_MANAGEMENT_DEBUG_LOG_OWNERSHIP_FAIL = 0,
    SLEEP_MANAGEMENT_DEBUG_LOG_MAX = 1,
    SLEEP_MANAGEMENT_DEBUG_LOG_DUMP = 255
} sleep_management_debug_log_index_t;


void deep_sleep_cmsys_backup(void);
void deep_sleep_cmsys_restore(void);

void sleep_management_register_suspend_callback(sleep_management_backup_restore_module_t module , sleep_management_suspend_callback_t callback , void *data);
void sleep_management_register_resume_callback(sleep_management_backup_restore_module_t module , sleep_management_resume_callback_t callback , void *data);
void sleep_management_suspend_callback(void);
void sleep_management_resume_callback(void);
uint8_t sleep_management_get_lock_handle(const char *handle_name);
void sleep_management_release_lock_handle(uint8_t handle_index);
void sleep_management_lock_sleep(sleep_management_lock_sleep_t lock, uint8_t handle_index);
bool sleep_management_check_sleep_locks(void);
bool sleep_management_check_handle_status(uint8_t handle_index);
uint32_t sleep_management_get_lock_sleep_request_info(void);
uint32_t sleep_management_get_lock_sleep_handle_list(void);
void sleep_management_low_power_init_setting(void);
int8_t hal_lp_connsys_get_own_enable_int(void);
int8_t hal_lp_connsys_give_n9_own(void);
void sleep_management_dump_sleep_handel_name(uint32_t handle_index);
void sleep_management_dump_debug_log(sleep_management_debug_log_index_t log_index);

#ifdef SLEEP_MANAGEMENT_DEBUG_ENABLE
void sleep_management_debug_lock_sleep_timelog(sleep_management_lock_sleep_t lock, uint8_t handle_index);
void sleep_management_debug_backup_restore_fun_timelog(uint32_t type, uint32_t order, uint32_t callback);
void sleep_management_debug_dump_lock_sleep_time(void);
void sleep_management_debug_reset_lock_sleep_time(void);
void sleep_management_debug_dump_backup_restore_time(void);
void sleep_management_dump_wakeup_source(uint32_t wakeup_source, uint32_t eint_status);
void sleep_management_dump_eint_wakeup_source(uint32_t eint_num);
#endif

#endif
#endif

