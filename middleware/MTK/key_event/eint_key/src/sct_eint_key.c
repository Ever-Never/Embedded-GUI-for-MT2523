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



#ifdef MTK_EINT_KEY_ENABLE
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <string.h>
#include <stdio.h>
#include "syslog.h"
#include <stdlib.h>
#include <assert.h>

#include "hal_log.h"
#include "task_def.h"
#include "hal_gpt.h"

#include "eint_key_custom.h"
#include "sct_key_event_internal.h"
#include "eint_key.h"

sct_eint_key_event_t sct_eint_key_event;

static void sct_eint_key_callback(bsp_eint_key_event_t event, uint8_t key_data, void *user_data)
{
	uint32_t time;

	hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &time);

	sct_eint_key_event.key_data   = key_data;
	sct_eint_key_event.state      = event;
	sct_eint_key_event.time_stamp = time;
	sct_eint_key_event.is_update  = true;
	
    sct_key_send_message(SCT_GET_KEY_DATA_EINT, NULL);
}

bool sct_eint_key_init(void)
{
	bsp_eint_key_config_t key_config;
	uint32_t debounce_time;
	

    key_config.longpress_time = sct_key_driven_signal_time;
    key_config.repeat_time    = sct_key_driven_signal_time;
    
    if (bsp_eint_key_init(&key_config) != true) {
        return false;
    }

    /*set debouce as 5ms, more sensitive*/

    #ifdef __CFW_CONFIG_SUPPORT__
	debounce_time = CFW_CFG_ITEM_VALUE(cfw_sct_key_debounce_time);
    #else
    debounce_time = SCT_KEY_DEFAULT_DEBOUNCE_TIME;
    #endif
    
    bsp_eint_key_set_debounce_time(debounce_time);

    /*register normoal keypad callback*/
    bsp_eint_key_register_callback(sct_eint_key_callback, NULL);

    bsp_eint_key_enable();
    
    return true;
}



#endif /*MTK_EINT_KEY_ENABLE*/

