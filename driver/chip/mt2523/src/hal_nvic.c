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

#include "hal_nvic.h"

#ifdef HAL_NVIC_MODULE_ENABLED
#include "hal_nvic_internal.h"
#include "hal_flash_disk_internal.h"
#include "memory_attribute.h"
#include "hal_log.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void (*nvic_callback)(hal_nvic_irq_t irq_number);
    uint32_t irq_pending;
} nvic_function_t;

static const uint32_t defualt_irq_priority[IRQ_NUMBER_MAX] = {
    OSTimer_IRQ_PRIORITY,       //External interrupt 0
    RESERVED_IRQ_PRIORITY,      //External interrupt 1
    MD_TOPSM_IRQ_PRIORITY,      //External interrupt 2
    CM4_TOPSM_IRQ_PRIORITY,     //External interrupt 3
    RESERVED_IRQ_PRIORITY,      //External interrupt 4
    RESERVED_IRQ_PRIORITY,      //External interrupt 5
    RESERVED_IRQ_PRIORITY,      //External interrupt 6
    RESERVED_IRQ_PRIORITY,      //External interrupt 7
    ACCDET_IRQ_PRIORITY,        //External interrupt 8
    RTC_IRQ_PRIORITY,           //External interrupt 9
    KP_IRQ_PRIORITY,            //External interrupt 10
    GPTTIMER_IRQ_PRIORITY,      //External interrupt 11
    EINT_IRQ_PRIORITY,          //External interrupt 12
    LCD_IRQ_PRIORITY,           //External interrupt 13
    LCD_AAL_IRQ_PRIORITY,       //External interrupt 14
    DSI_IRQ_PRIORITY,           //External interrupt 15
    RESIZE_IRQ_PRIORITY,        //External interrupt 16
    G2D_IRQ_PRIORITY,           //External interrupt 17
    CAM_IRQ_PRIORITY,           //External interrupt 18
    ROT_DMA_IRQ_PRIORITY,       //External interrupt 19
    SCAM_IRQ_PRIORITY,          //External interrupt 20
    DMA_IRQ_PRIORITY,           //External interrupt 21
    DMA_AO_IRQ_PRIORITY    ,    //External interrupt 22
    I2C_DUAL_IRQ_PRIORITY,      //External interrupt 23
    I2C0_IRQ_PRIORITY,          //External interrupt 24
    I2C1_IRQ_PRIORITY,          //External interrupt 25
    I2C2_IRQ_PRIORITY,          //External interrupt 26
    RESERVED_IRQ_PRIORITY,      //External interrupt 27
    GPCOUNTER_IRQ_PRIORITY,     //External interrupt 28
    UART0_IRQ_PRIORITY,         //External interrupt 29
    UART1_IRQ_PRIORITY,         //External interrupt 30
    UART2_IRQ_PRIORITY,         //External interrupt 31
    UART3_IRQ_PRIORITY,         //External interrupt 32
    USB20_IRQ_PRIORITY,         //External interrupt 33
    MSDC0_IRQ_PRIORITY,         //External interrupt 34
    MSDC1_IRQ_PRIORITY,         //External interrupt 35
    RESERVED_IRQ_PRIORITY,      //External interrupt 36
    RESERVED_IRQ_PRIORITY,      //External interrupt 37
    SF_IRQ_PRIORITY,            //External interrupt 38
    DSP22CPU_IRQ_PRIORITY,      //External interrupt 39
    SENSORDMA_IRQ_PRIORITY,     //External interrupt 40
    RGU_IRQ_PRIORITY,           //External interrupt 41
    SPI_SLV_IRQ_PRIORITY,       //External interrupt 42
    SPI_MST0_IRQ_PRIORITY,      //External interrupt 43
    SPI_MST1_IRQ_PRIORITY,      //External interrupt 44
    SPI_MST2_IRQ_PRIORITY,      //External interrupt 45
    SPI_MST3_IRQ_PRIORITY,      //External interrupt 46
    TRNG_IRQ_PRIORITY,          //External interrupt 47
    BT_TIMCON_IRQ_PRIORITY,     //External interrupt 48
    BTIF_IRQ_PRIORITY,          //External interrupt 49
    RESERVED_IRQ_PRIORITY,      //External interrupt 50
    RESERVED_IRQ_PRIORITY,      //External interrupt 51
    RESERVED_IRQ_PRIORITY,      //External interrupt 52
    RESERVED_IRQ_PRIORITY,      //External interrupt 53
    RESERVED_IRQ_PRIORITY,      //External interrupt 54
    RESERVED_IRQ_PRIORITY,      //External interrupt 55
    RESERVED_IRQ_PRIORITY,      //External interrupt 56
    RESERVED_IRQ_PRIORITY,      //External interrupt 57
    RESERVED_IRQ_PRIORITY,      //External interrupt 58
    RESERVED_IRQ_PRIORITY,      //External interrupt 59
    RESERVED_IRQ_PRIORITY,      //External interrupt 60
    RESERVED_IRQ_PRIORITY,      //External interrupt 61
    RESERVED_IRQ_PRIORITY,      //External interrupt 62
    RESERVED_IRQ_PRIORITY,      //External interrupt 63
};

nvic_function_t nvic_function_table[IRQ_NUMBER_MAX];

static uint32_t get_pending_irq()
{
    return ((SCB->ICSR & SCB_ICSR_ISRPENDING_Msk) >> SCB_ICSR_ISRPENDING_Pos);
}

hal_nvic_status_t hal_nvic_init(void)
{
    static uint32_t priority_set = 0;
    uint32_t i;

    if (priority_set == 0) {
        /* Set defualt priority only one time */
        for (i = 0; i < IRQ_NUMBER_MAX; i++) {
            NVIC_SetPriority((hal_nvic_irq_t)i, defualt_irq_priority[i]);
        }
        priority_set = 1;
    }
    return HAL_NVIC_STATUS_OK;
}

hal_nvic_status_t hal_nvic_enable_irq(hal_nvic_irq_t irq_number)
{
    hal_nvic_status_t status = HAL_NVIC_STATUS_ERROR;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        status = HAL_NVIC_STATUS_ERROR_IRQ_NUMBER;
        return status;
    } else {
        NVIC_EnableIRQ(irq_number);
        status = HAL_NVIC_STATUS_OK;
    }

    return status;
}

hal_nvic_status_t hal_nvic_disable_irq(hal_nvic_irq_t irq_number)
{
    hal_nvic_status_t status = HAL_NVIC_STATUS_ERROR;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        status = HAL_NVIC_STATUS_ERROR_IRQ_NUMBER;
        return status;
    } else {
        NVIC_DisableIRQ(irq_number);
        status = HAL_NVIC_STATUS_OK;
    }

    return status;
}

uint32_t hal_nvic_get_pending_irq(hal_nvic_irq_t irq_number)
{
    uint32_t ret = 0xFF;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        return ret;
    } else {
        ret = NVIC_GetPendingIRQ(irq_number);
    }

    return ret;
}

hal_nvic_status_t hal_nvic_set_pending_irq(hal_nvic_irq_t irq_number)
{
    hal_nvic_status_t status = HAL_NVIC_STATUS_ERROR;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        status = HAL_NVIC_STATUS_ERROR_IRQ_NUMBER;
        return status;
    } else {
        NVIC_SetPendingIRQ(irq_number);
        status = HAL_NVIC_STATUS_OK;
    }

    return status;
}

hal_nvic_status_t hal_nvic_clear_pending_irq(hal_nvic_irq_t irq_number)
{
    hal_nvic_status_t status = HAL_NVIC_STATUS_ERROR;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        status = HAL_NVIC_STATUS_ERROR_IRQ_NUMBER;
        return status;
    } else {
        NVIC_ClearPendingIRQ(irq_number);
        status = HAL_NVIC_STATUS_OK;
    }

    return status;
}

hal_nvic_status_t hal_nvic_set_priority(hal_nvic_irq_t irq_number, uint32_t priority)
{
    hal_nvic_status_t status = HAL_NVIC_STATUS_ERROR;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        status = HAL_NVIC_STATUS_ERROR_IRQ_NUMBER;
        return status;
    } else {
        NVIC_SetPriority(irq_number, priority);
        status = HAL_NVIC_STATUS_OK;
    }

    return status;
}

uint32_t hal_nvic_get_priority(hal_nvic_irq_t irq_number)
{
    uint32_t ret = RESERVED_IRQ_PRIORITY;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        return ret;
    } else {
        ret = NVIC_GetPriority(irq_number);
    }

    return ret;
}

static uint32_t get_current_irq()
{
    uint32_t irq_num = ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) >> SCB_ICSR_VECTACTIVE_Pos);
    return (irq_num - 16);
}

ATTR_TEXT_IN_TCM hal_nvic_status_t isrC_main()
{
    hal_nvic_status_t status = HAL_NVIC_STATUS_ERROR;
    hal_nvic_irq_t irq_number;

    Flash_ReturnReady();

    irq_number = (hal_nvic_irq_t)(get_current_irq());
    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        status = HAL_NVIC_STATUS_ERROR_IRQ_NUMBER;
        return status;
    } else if (nvic_function_table[irq_number].nvic_callback == NULL) {
        status = HAL_NVIC_STATUS_ERROR_NO_ISR;
        log_hal_error("ERROR: no IRQ handler! \n");
        return status;
    } else {
        nvic_function_table[irq_number].irq_pending = get_pending_irq();
        nvic_function_table[irq_number].nvic_callback(irq_number);
        status = HAL_NVIC_STATUS_OK;
    }

    return status;
}

hal_nvic_status_t hal_nvic_register_isr_handler(hal_nvic_irq_t irq_number, hal_nvic_isr_t callback)
{
    uint32_t mask;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX || callback == NULL) {
        return HAL_NVIC_STATUS_INVALID_PARAMETER;
    }

    mask = save_and_set_interrupt_mask();
    NVIC_ClearPendingIRQ(irq_number);
    nvic_function_table[irq_number].nvic_callback = callback;
    nvic_function_table[irq_number].irq_pending = 0;
    restore_interrupt_mask(mask);

    return HAL_NVIC_STATUS_OK;
}

hal_nvic_status_t hal_nvic_save_and_set_interrupt_mask(uint32_t *mask)
{
    *mask = save_and_set_interrupt_mask();
    return HAL_NVIC_STATUS_OK;
}

hal_nvic_status_t hal_nvic_restore_interrupt_mask(uint32_t mask)
{
    restore_interrupt_mask(mask);
    return HAL_NVIC_STATUS_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* HAL_NVIC_MODULE_ENABLED */

