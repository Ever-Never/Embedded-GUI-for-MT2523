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

#include "hal_audio_custom.h"

#if defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED)

extern afe_t afe;

void afe_switch_external_amp(bool on)
{
    hal_gpio_status_t ret;
    hal_pinmux_status_t ret_pinmux_status;
    extern const char BSP_SPEAKER_EBABLE_PIN;
    ret = hal_gpio_init((hal_gpio_pin_t)BSP_SPEAKER_EBABLE_PIN);
    if (ret != HAL_GPIO_STATUS_OK) {
        return;
    }
    ret_pinmux_status = hal_pinmux_set_function((hal_gpio_pin_t)BSP_SPEAKER_EBABLE_PIN, 0);
    if (ret_pinmux_status != HAL_PINMUX_STATUS_OK) {
        return;
    }
    ret = hal_gpio_set_direction((hal_gpio_pin_t)BSP_SPEAKER_EBABLE_PIN, HAL_GPIO_DIRECTION_OUTPUT);
    if (ret != HAL_GPIO_STATUS_OK) {
        return;
    }

    if (on && (!afe.ext_pa_on)) {
        ret = hal_gpio_set_output((hal_gpio_pin_t)BSP_SPEAKER_EBABLE_PIN, HAL_GPIO_DATA_HIGH);
        if (ret != HAL_GPIO_STATUS_OK) {
            return;
        }
        afe.ext_pa_on = true;
    } else {
        if (!afe.ext_pa_on) {
            return;
        }
        afe_volume_ramp_down();
        ret = hal_gpio_set_output((hal_gpio_pin_t)BSP_SPEAKER_EBABLE_PIN, HAL_GPIO_DATA_LOW);
        if (ret != HAL_GPIO_STATUS_OK) {
            return;
        }
        afe.ext_pa_on = false;
    }
    ret = hal_gpio_deinit((hal_gpio_pin_t)BSP_SPEAKER_EBABLE_PIN);
    if (ret != HAL_GPIO_STATUS_OK) {
        return;
    }
}

#endif  /* defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED) */
