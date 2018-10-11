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

#include "eint_key_custom.h"
#include "eint_key.h"
#include "hal_gpio.h"
#include "hal_log.h"
#include "assert.h"

extern const char BSP_EINTKEY0_PIN;
extern const char BSP_EINTKEY1_PIN;
extern const char BSP_EINTKEY2_PIN;

extern const char BSP_EINTKEY0_PIN_M_EINT;
extern const char BSP_EINTKEY1_PIN_M_EINT;
extern const char BSP_EINTKEY2_PIN_M_EINT;

extern const unsigned char BSP_EINTKEY0_EINT;
extern const unsigned char BSP_EINTKEY1_EINT;
extern const unsigned char BSP_EINTKEY2_EINT;


/*config the GPIO, eint mode, eint number, keyad data */
bsp_eint_key_mapping_t eint_key_mapping[BSP_EINT_KEY_NUMBER] = {
    {HAL_GPIO_0,  HAL_GPIO_0_EINT0, HAL_EINT_NUMBER_0, BSP_EINT_KEY_DATA0},
};



void eint_key_custom_init(void)
{

#ifdef EINT_KEY_USED_EPT_CONFIGURATION
    uint32_t i;

    if ((BSP_EINTKEY0_PIN == 0xff) && (BSP_EINTKEY1_PIN == 0xff) && (BSP_EINTKEY2_PIN == 0xff)) {
        log_hal_info("[eint_key]eint key pin has not been configured by ept tool\r\n");
        assert(0);
    }
    
    eint_key_mapping[0].gpio_port = BSP_EINTKEY0_PIN;
    eint_key_mapping[1].gpio_port = BSP_EINTKEY1_PIN;
    eint_key_mapping[2].gpio_port = BSP_EINTKEY2_PIN;

    eint_key_mapping[0].eint_mode = BSP_EINTKEY0_PIN_M_EINT;
    eint_key_mapping[1].eint_mode = BSP_EINTKEY1_PIN_M_EINT;
    eint_key_mapping[2].eint_mode = BSP_EINTKEY2_PIN_M_EINT;

    eint_key_mapping[0].eint_number = BSP_EINTKEY0_EINT;
    eint_key_mapping[1].eint_number = BSP_EINTKEY1_EINT;
    eint_key_mapping[2].eint_number = BSP_EINTKEY2_EINT;

    eint_key_mapping[0].key_data = BSP_EINT_KEY_DATA0;
    eint_key_mapping[1].key_data = BSP_EINT_KEY_DATA1;
    eint_key_mapping[2].key_data = BSP_EINT_KEY_DATA2;

    for (i=0; i<BSP_EINT_KEY_NUMBER; i++) {
        log_hal_info("[eint_key]led%d: gpio = %d; mode = %d; eint=%d; key_data = %d\r\n", \
             i, \
             eint_key_mapping[i].gpio_port, \
             eint_key_mapping[i].eint_mode, \
             eint_key_mapping[i].eint_number, \
             eint_key_mapping[i].key_data);
    }
#endif

}


