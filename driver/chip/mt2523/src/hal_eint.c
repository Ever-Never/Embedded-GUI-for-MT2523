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

/*************************************************************************
 * Included header files
 *************************************************************************/
#include "hal_eint.h"

#ifdef HAL_EINT_MODULE_ENABLED
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "hal_log.h"
#include "hal_gpt.h"
#include "memory_attribute.h"
#include "hal_eint_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************
 * Define global variables
 *************************************************************************/
ATTR_RWDATA_IN_TCM EINT_REGISTER_T *EINT_REGISTER = (EINT_REGISTER_T *)EINT_BASE;
static eint_function_t eint_function_table[HAL_EINT_NUMBER_MAX];

/*************************************************************************
 * Define function prototpye
 *************************************************************************/
static void hal_eint_isr(uint32_t index);

static void eint_enable_domain(uint32_t eint_no, eint_domain_status_t enable)
{
    if (enable == EINT_DOMAIN_DISABLE) {
        EINT_REGISTER->EINT_DOMEN &= ~(1 << eint_no);
    } else {
        EINT_REGISTER->EINT_DOMEN |= (enable << eint_no);
    }
}

static uint32_t eint_get_status(void)
{
    return (EINT_REGISTER->EINT_STA);
}

void eint_ack_interrupt(uint32_t eint_number)
{
    EINT_REGISTER->EITN_INTACK = (1 << eint_number);
}

static uint32_t eint_caculate_debounce_time(uint32_t ms)
{
    uint32_t prescaler;
    uint32_t count;

    if (ms == 0) {
        /* set to one 32KHz clock cycle */
        prescaler = EINT_CON_PRESCALER_32KHZ;
        count = 0;
    } else if (ms <= 62) {
        prescaler = EINT_CON_PRESCALER_32KHZ;
        count = (ms << 5) + ms;
    } else if (ms <= 125) {
        prescaler = EINT_CON_PRESCALER_16KHZ;
        count = (ms << 4) + (ms >> 1);
    } else if (ms <= 250) {
        prescaler = EINT_CON_PRESCALER_8KHZ;
        count = (ms << 3) + (ms >> 2);
    } else if (ms <= 500) {
        prescaler = EINT_CON_PRESCALER_4KHZ;
        count = (ms << 2) + (ms >> 3);
    } else if (ms <= 1000) {
        prescaler = EINT_CON_PRESCALER_2KHZ;
        count = (ms << 1) + (ms >> 4);
    } else if (ms <= 2000) {
        prescaler = EINT_CON_PRESCALER_1KHZ;
        count = ms + (ms >> 5);
    } else if (ms <= 4000) {
        prescaler = EINT_CON_PRESCALER_512HZ;
        count = (ms >> 1) + (ms >> 6);
    } else if (ms <= 8000) {
        prescaler = EINT_CON_PRESCALER_256HZ;
        count = (ms >> 2) + (ms >> 7);
    } else {
        /* set to maximum prescaler/count */
        prescaler = EINT_CON_PRESCALER_256HZ;
        count = EINT_CON_DBC_CNT_MASK;
    }

    if (count > EINT_CON_DBC_CNT_MASK) {
        count = EINT_CON_DBC_CNT_MASK;
    }

    count = (count | EINT_CON_DBC_EN_MASK |
             (EINT_CON_PRESCALER_MASK & (prescaler << EINT_CON_PRESCALER_OFFSET)));
    return count;
}

hal_eint_status_t hal_eint_init(hal_eint_number_t eint_number, const hal_eint_config_t *eint_config)
{
    hal_eint_status_t status;
    uint32_t mask;
    static uint32_t ini_state = 0;

    if (ini_state == 0) {
        hal_nvic_register_isr_handler(EINT_IRQn, (hal_nvic_isr_t)hal_eint_isr);
        hal_nvic_enable_irq(EINT_IRQn);
        ini_state = 1;
    }

    if (eint_number >= HAL_EINT_NUMBER_MAX || eint_config == NULL) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    mask = save_and_set_interrupt_mask();
    eint_function_table[eint_number].eint_callback = NULL;
    eint_function_table[eint_number].user_data = NULL;

    status = hal_eint_set_trigger_mode(eint_number, eint_config->trigger_mode);
    status |= hal_eint_set_debounce_time(eint_number, eint_config->debounce_time);

    eint_ack_interrupt(eint_number);
    /* enable EINT domain */
    eint_enable_domain(eint_number, EINT_DOMAIN_ENABLE);
    restore_interrupt_mask(mask);
    return status;
}

hal_eint_status_t hal_eint_deinit(hal_eint_number_t eint_number)
{
    uint32_t mask;

    if (eint_number >= HAL_EINT_NUMBER_MAX) {
        return HAL_EINT_STATUS_ERROR_EINT_NUMBER;
    }

    mask = save_and_set_interrupt_mask();
    eint_function_table[eint_number].eint_callback = NULL;
    eint_function_table[eint_number].user_data = NULL;

    eint_ack_interrupt(eint_number);
    hal_eint_mask(eint_number);
    restore_interrupt_mask(mask);
    return HAL_EINT_STATUS_OK;
}

hal_eint_status_t hal_eint_register_callback(hal_eint_number_t eint_number,
        hal_eint_callback_t eint_callback,
        void *user_data)
{
    uint32_t mask;

    if (eint_number >= HAL_EINT_NUMBER_MAX || eint_callback == NULL) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    mask = save_and_set_interrupt_mask();
    eint_function_table[eint_number].eint_callback = eint_callback;
    eint_function_table[eint_number].user_data = user_data;
    restore_interrupt_mask(mask);
    return HAL_EINT_STATUS_OK;
}


/***************************************** *******************************
    sensitivity:
                            1         level  (default)
                            0         edge
    polarity:               0         negative polarity  (default)
                            1         positive polarity

 *************************************************************************/
hal_eint_status_t hal_eint_set_trigger_mode(hal_eint_number_t eint_number,
        hal_eint_trigger_mode_t trigger_mode)
{
    uint32_t mask;

    if (eint_number >= HAL_EINT_NUMBER_MAX) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    mask = save_and_set_interrupt_mask();
    if (trigger_mode == HAL_EINT_LEVEL_LOW) {
        (EINT_REGISTER->EINT_SENS_SET) |= (1 << eint_number);
        (EINT_REGISTER->EINT_CON[eint_number].CON_REGISTER.EINT_CON_W) &= (~EINT_CON_POL_MASK);
    } else if (trigger_mode == HAL_EINT_LEVEL_HIGH) {
        (EINT_REGISTER->EINT_SENS_SET) |= (1 << eint_number) ;
        (EINT_REGISTER->EINT_CON[eint_number].CON_REGISTER.EINT_CON_W) |= EINT_CON_POL_MASK;
    } else if (trigger_mode == HAL_EINT_EDGE_FALLING) {
        (EINT_REGISTER->EINT_SENS_CLR) |= (1 << eint_number);
        (EINT_REGISTER->EINT_CON[eint_number].CON_REGISTER.EINT_CON_W) &= (~EINT_CON_POL_MASK);
    } else if (trigger_mode == HAL_EINT_EDGE_RISING) {
        (EINT_REGISTER->EINT_SENS_CLR) |= (1 << eint_number);
        (EINT_REGISTER->EINT_CON[eint_number].CON_REGISTER.EINT_CON_W) |= EINT_CON_POL_MASK;
    } else if (trigger_mode == HAL_EINT_EDGE_FALLING_AND_RISING) {
        /* DUALEGE depends on SENS */
        (EINT_REGISTER->EINT_SENS_CLR) |= (1 << eint_number);
        (EINT_REGISTER->EINT_DUALEDGE_SENS_SET) |= (1 << eint_number);
    } else {
        restore_interrupt_mask(mask);
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    restore_interrupt_mask(mask);
    return HAL_EINT_STATUS_OK;
}

hal_eint_status_t hal_eint_set_debounce_time(hal_eint_number_t eint_number, uint32_t time_ms)
{
    uint32_t mask, count, eint_con;

    if (eint_number >= HAL_EINT_NUMBER_MAX) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    count = eint_caculate_debounce_time(time_ms);
    mask = save_and_set_interrupt_mask();
    eint_con = EINT_REGISTER->EINT_CON[eint_number].CON_REGISTER.EINT_CON_W;
    eint_con &= (~EINT_CON_DBC_CNT_MASK);
    eint_con |= count;

    if (time_ms == 0) {
        //disenable debounce bit
        eint_con &= EINT_CON_DEBOUNCE_MASK;
        EINT_REGISTER->EINT_CON[eint_number].CON_REGISTER.EINT_CON_W = eint_con;
        /* can't reset debounce as it will cause 1/32k delay by debounce */
    }
    else {
        EINT_REGISTER->EINT_CON[eint_number].CON_REGISTER.EINT_CON_W = eint_con & (~EINT_CON_DBC_CNT_MASK);
        hal_gpt_delay_us(125);
        
        EINT_REGISTER->EINT_CON[eint_number].CON_REGISTER.EINT_CON_W = eint_con;
        EINT_REGISTER->EINT_CON[eint_number].CON_REGISTER.EINT_CON_W |= EINT_CON_RSTD_MASK;
    }

    restore_interrupt_mask(mask);
    
    return HAL_EINT_STATUS_OK;
}

/*This API only used for debounce time less than 1ms.*/
hal_eint_status_t hal_eint_set_debounce_count(hal_eint_number_t eint_number, uint32_t count)
{
    uint32_t mask, eint_con;
    uint32_t prescaler;

    if (eint_number >= HAL_EINT_NUMBER_MAX) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    if ((count & (~EINT_CON_DBC_CNT_MASK))) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    /*set clock source alway is 32K.*/
    prescaler = EINT_CON_PRESCALER_32KHZ;

    mask = save_and_set_interrupt_mask();
    eint_con = EINT_REGISTER->EINT_CON[eint_number].CON_REGISTER.EINT_CON_W;
    eint_con &= (~EINT_CON_DBC_CNT_MASK);
    eint_con |= count;
    restore_interrupt_mask(mask);
    
    if (count == 0) {
        //disenable debounce bit
        eint_con &= EINT_CON_DEBOUNCE_MASK;
        EINT_REGISTER->EINT_CON[eint_number].CON_REGISTER.EINT_CON_W = eint_con;
        /* can't reset debounce as it will cause 1/32k delay by debounce */
    } else {
        /*Enable debounce, maybe debounce is disable.*/
        EINT_REGISTER->EINT_CON[eint_number].CON_REGISTER.EINT_CON_W = eint_con & (~EINT_CON_DBC_CNT_MASK);
        hal_gpt_delay_us(125);
        
        eint_con |= EINT_CON_DBC_EN_MASK;
        eint_con = (eint_con & (~EINT_CON_PRESCALER_MASK)) | (prescaler << EINT_CON_PRESCALER_OFFSET);
        
        EINT_REGISTER->EINT_CON[eint_number].CON_REGISTER.EINT_CON_W = eint_con;
        EINT_REGISTER->EINT_CON[eint_number].CON_REGISTER.EINT_CON_W |= EINT_CON_RSTD_MASK;
    }

    return HAL_EINT_STATUS_OK;
    
}

hal_eint_status_t hal_eint_set_software_trigger(hal_eint_number_t eint_number)
{
    uint32_t mask;

    if (eint_number >= HAL_EINT_NUMBER_MAX) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    mask = save_and_set_interrupt_mask();
    EINT_REGISTER->EINT_SOFT_SET |= (1 << eint_number);
    restore_interrupt_mask(mask);
    return HAL_EINT_STATUS_OK;
}

hal_eint_status_t hal_eint_clear_software_trigger(hal_eint_number_t eint_number)
{
    uint32_t mask;

    if (eint_number >= HAL_EINT_NUMBER_MAX) {
        return HAL_EINT_STATUS_ERROR;
    }

    mask = save_and_set_interrupt_mask();
    EINT_REGISTER->EINT_SOFT_CLR |= (1 << eint_number);
    restore_interrupt_mask(mask);
    return HAL_EINT_STATUS_OK;
}

hal_eint_status_t eint_mask_wakeup_source(hal_eint_number_t eint_number)
{
    uint32_t mask;
    if (eint_number >= HAL_EINT_NUMBER_MAX) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    mask = save_and_set_interrupt_mask();
    EINT_REGISTER->EINT_WAKEUP_MASK_SET |= (1 << eint_number);
    restore_interrupt_mask(mask);
    return HAL_EINT_STATUS_OK;
}

hal_eint_status_t eint_unmask_wakeup_source(hal_eint_number_t eint_number)
{
    uint32_t mask;

    if (eint_number >= HAL_EINT_NUMBER_MAX) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    mask = save_and_set_interrupt_mask();
    EINT_REGISTER->EINT_WAKEUP_MASK_CLR |= (1 << eint_number);
    restore_interrupt_mask(mask);
    return HAL_EINT_STATUS_OK;
}

//#ifdef DRV_FEATURE_EINT_MASK_SUPPORT
#if 1
hal_eint_status_t hal_eint_mask(hal_eint_number_t eint_number)
{
    uint32_t mask;
    if (eint_number >= HAL_EINT_NUMBER_MAX) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    mask = save_and_set_interrupt_mask();
    EINT_REGISTER->EINT_MASK_SET |= (1 << eint_number);
    eint_mask_wakeup_source(eint_number);
    restore_interrupt_mask(mask);
    return HAL_EINT_STATUS_OK;
}

hal_eint_status_t hal_eint_unmask(hal_eint_number_t eint_number)
{
    uint32_t mask;

    if (eint_number >= HAL_EINT_NUMBER_MAX) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    mask = save_and_set_interrupt_mask();
    EINT_REGISTER->EINT_MASK_CLR |= (1 << eint_number);
    eint_unmask_wakeup_source(eint_number);
    restore_interrupt_mask(mask);
    return HAL_EINT_STATUS_OK;
}
#endif


static void hal_eint_isr(uint32_t index)
{
    uint32_t status;
    uint32_t eint_index = 0;

    status = eint_get_status();
    if (status == 0) {
        return;
    }

    for (eint_index = 0; eint_index < HAL_EINT_NUMBER_MAX; eint_index++) {
        if (status & (1 << eint_index)) {
            hal_eint_mask((hal_eint_number_t)eint_index);
            eint_ack_interrupt(eint_index);
            if (eint_function_table[eint_index].eint_callback) {
                eint_function_table[eint_index].eint_callback(eint_function_table[eint_index].user_data);
            } else {
                log_hal_error("ERROR: no EINT interrupt handler!\n");
            }
        }
    }
}


#ifdef __cplusplus
}
#endif

#endif /* HAL_EINT_MODULE_ENABLED */

