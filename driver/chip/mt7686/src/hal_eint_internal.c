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
#include "hal_eint_internal.h"
#include "hal_nvic_internal.h"
#include "hal_log.h"
#include "hal_gpt.h"
#include "memory_attribute.h"


ATTR_RWDATA_IN_TCM EINT_REGISTER_T *EINT_REGISTER = (EINT_REGISTER_T *)EINT_BASE;
eint_function_t eint_function_table[HAL_EINT_NUMBER_MAX];

uint32_t eint_get_status(void)
{
    return (EINT_REGISTER->EINT_STA);
}

void eint_ack_interrupt(uint32_t eint_number)
{
    EINT_REGISTER->EITN_INTACK = (1 << eint_number);
}

uint32_t eint_caculate_debounce_time(uint32_t ms)
{
    uint32_t prescaler;
    uint32_t count;

    if (ms == 0) {
        /* set to one 32KHz clock cycle */
        prescaler = EINT_CON_PRESCALER_32KHZ;
        count = 0;
    } else if (ms <= 62) {
        prescaler = EINT_CON_PRESCALER_32KHZ;
        count = EINT_TIME_MS_TO_COUNT(ms,32768);
    } else if (ms <= 125) {
        prescaler = EINT_CON_PRESCALER_16KHZ;
        count = EINT_TIME_MS_TO_COUNT(ms,16384);
    } else if (ms <= 250) {
        prescaler = EINT_CON_PRESCALER_8KHZ;
        count = EINT_TIME_MS_TO_COUNT(ms,8192);
    } else if (ms <= 500) {
        prescaler = EINT_CON_PRESCALER_4KHZ;
        count = EINT_TIME_MS_TO_COUNT(ms,4096);
    } else if (ms <= 1000) {
        prescaler = EINT_CON_PRESCALER_2KHZ;
        count = EINT_TIME_MS_TO_COUNT(ms,2048);
    } else if (ms <= 2000) {
        prescaler = EINT_CON_PRESCALER_1KHZ;
        count = EINT_TIME_MS_TO_COUNT(ms,1024);
    } else if (ms <= 4000) {
        prescaler = EINT_CON_PRESCALER_512HZ;
        count = EINT_TIME_MS_TO_COUNT(ms,512);
    } else if (ms <= 8000) {
        prescaler = EINT_CON_PRESCALER_256HZ;
        count = EINT_TIME_MS_TO_COUNT(ms,256);
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

void hal_eint_isr(uint32_t index)
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
                //log_hal_error("ERROR: no EINT interrupt handler!\n");
            }
        }
    }
}

#endif /* HAL_EINT_MODULE_ENABLED */

