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

#include "hal_trng_internal.h"

#ifdef HAL_TRNG_MODULE_ENABLED
#include "hal_clock.h"

/*trng  base address register global variable*/
TRNG_REGISTER_T  *trng = TRNG;


void  trng_init(void)
{

    /* enable clock pdn*/
    hal_clock_enable(HAL_CLOCK_CG_SW_TRNG);

}

Trng_Result trng_config_timeout_limit(uint32_t timeout_value)

{
    /*time out shuld be less than TIMEOUT_VALUE*/

    uint32_t  timeout_limit = timeout_value;

    if (timeout_limit > TIMEOUT_VALUE) {
        return TRNG_GEN_FAIL;
    }

    trng->TRNG_CONF &= ~(0xFFF << (16));
    trng->TRNG_CONF |= (timeout_value << (16));

    return  TRNG_GEN_SUCCESS;
}


Trng_Result trng_enable_mode(bool H_FIRO, bool H_RO, bool H_GARO)
{

    if (true == H_FIRO) {
        trng->TRNG_CONF |= TRNG_H_FIRO_EN_MASK;
    } else {
        trng->TRNG_CONF &= ~TRNG_H_FIRO_EN_MASK;
    }
    if (true == H_RO) {
        trng->TRNG_CONF |= TRNG_H_RO_EN_MASK;
    } else {
        trng->TRNG_CONF &= ~TRNG_H_RO_EN_MASK;
    }

    if (true == H_GARO) {
        trng->TRNG_CONF |= TRNG_H_GARO_EN_MASK;
    } else {
        trng->TRNG_CONF &= ~TRNG_H_GARO_EN_MASK;
    }

    return  TRNG_GEN_SUCCESS;

}

void  trng_start(void)
{

    trng->TRNG_CTRL |= TRNG_START_MASK;

}

void  trng_stop(void)
{
    volatile uint32_t temp_random = 0;
    temp_random = trng->TRNG_DATA;
    temp_random += 1;

    trng->TRNG_CTRL &= ~TRNG_START_MASK;


}


void  trng_deinit(void)

{
    /*disable clock pdn*/
    hal_clock_disable(HAL_CLOCK_CG_SW_TRNG);

}


uint32_t  trng_get_random_data(void)
{

    volatile uint32_t random_data = 0;
    volatile uint32_t ready_data = 0;

    ready_data = trng->TRNG_INT_SET;
    ready_data &= TRNG_RDY_MASK;
    while (ready_data != TRNG_RDY_MASK) {
        ready_data = trng->TRNG_INT_SET;
	if (ready_data == TRNG_TIMEOUT_MASK) {
           return 0;
		}
        ready_data &= TRNG_RDY_MASK;
    }
    trng->TRNG_INT_CLR = TRNG_INT_CLR_MASK;
    trng->TRNG_CTRL = TRNG_STOP_MASK;
    random_data = trng->TRNG_DATA;

    return random_data;
}

#endif /*HAL_TRNG_MODULE_ENABLED*/







