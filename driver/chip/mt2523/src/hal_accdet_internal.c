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

#include "hal_accdet.h"

#ifdef HAL_ACCDET_MODULE_ENABLED

#include "hal_accdet_internal.h"
#include "assert.h"
#include "hal_nvic.h"

#ifdef __cplusplus
extern "C" {
#endif

extern volatile hal_accdet_callback_event_t g_accdet_last_event;
extern accdet_callback_t g_accdet_call_back;

#ifdef ACCDET_EINT_SUPPORT

extern volatile bool g_accdet_is_plug_in;
extern const unsigned char HAL_ACCDET_EINT;

//refer to AUX_EINT2_HISR()

void accdet_eint_callback(void *user_data)
{
    hal_eint_mask((hal_eint_number_t) HAL_ACCDET_EINT);

    g_accdet_is_plug_in = !g_accdet_is_plug_in;

    /* Plug in */
    if (g_accdet_is_plug_in == true) {
        /* Set EINT rising edge trigger */
        hal_eint_set_trigger_mode((hal_eint_number_t) HAL_ACCDET_EINT, HAL_EINT_EDGE_RISING);

        /* Enable ACCDET */
        ACCDET->ACCDET_CTRL |= ACCDET_CTRL_ACCDET_EN_MASK;
    } else {/* Plug out */
        /* Set EINT falling edge trigger */
        hal_eint_set_trigger_mode((hal_eint_number_t) HAL_ACCDET_EINT, HAL_EINT_EDGE_FALLING);
    }

    hal_eint_unmask((hal_eint_number_t) HAL_ACCDET_EINT);
}

void accdet_eint_callback_register(void)
{
    hal_eint_config_t eint_config;

    /* Make sure pin for EINT of ACCDET is configured by EPT tool or by calling hal_pinmux_set_function() directly */
    if (0xff == HAL_ACCDET_EINT) {
        assert(0);
    }

    /* Set EINT falling edge trigger */
    eint_config.trigger_mode = HAL_EINT_EDGE_FALLING;

    /* Set debounce time for EINT */
    eint_config.debounce_time = HAL_ACCDET_EINT_DEBOUNCE;

    /* EINT init */
    hal_eint_init((hal_eint_number_t) HAL_ACCDET_EINT, &eint_config);

    /* Register EINT callback */
    hal_eint_register_callback((hal_eint_number_t) HAL_ACCDET_EINT, accdet_eint_callback, NULL);

    /* Unmask EINT */
    hal_eint_unmask((hal_eint_number_t) HAL_ACCDET_EINT);

    /* Enable EINT IRQ */
    //NVIC_EnableIRQ(EINT_IRQn);
}

void accdet_eint_callback_unregister(void)
{
    /* Deinit EINT */
    hal_eint_deinit((hal_eint_number_t) HAL_ACCDET_EINT);
}

#endif//ACCDET_EINT_SUPPORT

void accdet_irq_callback(hal_nvic_irq_t irq_number)
{
    hal_accdet_callback_t hal_accdet_callback;
    void *arg;
    hal_accdet_callback_event_t new_event;
    uint32_t accdet_irq_status;

    /* Clear IRQ flag */
    ACCDET->ACCDET_IRQ_STS |= ACCDET_IRQ_STS_IRQ_CLR_MASK;

    /* Dummy read to clear the irq flag */
    accdet_irq_status = ACCDET->ACCDET_IRQ_STS;
    accdet_irq_status = accdet_irq_status;

    /* Get callback function and parameter */
    hal_accdet_callback = g_accdet_call_back.func;
    arg = g_accdet_call_back.arg;

    /* Get event of ACCDET*/
    new_event = (hal_accdet_callback_event_t)(ACCDET->ACCDET_MEMORIZED_IN & 3);

    /* If plug-in with key pressed, sent event HAL_ACCDET_EVENT_PLUG_IN instead of HAL_ACCDET_EVENT_HOOK_KEY */
    if ((g_accdet_last_event == HAL_ACCDET_EVENT_PLUG_OUT) && (new_event == HAL_ACCDET_EVENT_HOOK_KEY)) {
        new_event = HAL_ACCDET_EVENT_PLUG_IN;
    }

#ifdef ACCDET_EINT_SUPPORT
    /* If event is HAL_ACCDET_EVENT_PLUG_OUT, disable ACCDET for low power consumption */
    if (new_event == HAL_ACCDET_EVENT_PLUG_OUT) {
        ACCDET->ACCDET_CTRL &= ~ACCDET_CTRL_ACCDET_EN_MASK;
    }
#endif

    /* Update g_accdet_last_event as current event */
    g_accdet_last_event = new_event;

    /* Callback function */
    hal_accdet_callback((hal_accdet_callback_event_t)new_event, arg);
}

void accdet_irq_callback_register(void)
{
    /* Register IRQ handler */
    hal_nvic_register_isr_handler(ACCDET_IRQn, accdet_irq_callback);

    /* Enable ACCDET IRQ */
    hal_nvic_enable_irq(ACCDET_IRQn);
}

void accdet_irq_callback_unregister(void)
{
    /* Disable ACCDET IRQ */
    hal_nvic_disable_irq(ACCDET_IRQn);

    /* Set callback and parameter as NULL */
    g_accdet_call_back.func = NULL;
    g_accdet_call_back.arg = NULL;
}

#ifdef __cplusplus
}
#endif

#endif /* HAL_ACCDET_MODULE_ENABLED */

