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

#include "bt_sink_srv.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_state_notify.h"
#include "bt_sink_srv_action.h"
#include "bt_sink_srv_common.h"
#include "bt_device_manager.h"
#include "bt_sink_srv_audio_setting.h"

#ifdef __BT_SINK_SRV_TRACE__
const static char *bt_sink_srv_key_value_strings[] = {
    "KEY_NONE",
    "KEY_FUNC",
    "KEY_NEXT",
    "KEY_PREV",
    "KEY_VOL_DOWN",
    "KEY_VOL_UP"
};

const static char *bt_sink_srv_key_action_strings[] = {
    "ACT_NONE",
    "ACT_PRESS_DOWN",
    "ACT_PRESS_UP",
    "ACT_LONG_PRESS_DOWN",
    "ACT_LONG_PRESS_UP",
    "ACT_LONG_LONG_PRESS_DOWN",
    "ACT_LONG_LONG_PRESS_UP",
    "ACT_DOUBLE_CLICK"
};
#endif

extern void bt_sink_srv_atci_init(void);

bt_sink_srv_status_t bt_sink_srv_key_action(bt_sink_srv_key_value_t key_value,
        bt_sink_srv_key_action_t key_action)
{
    const bt_sink_srv_table_t *mapping_table = bt_sink_srv_get_mapping_table();
    bt_sink_srv_status_t result = BT_SINK_SRV_STATUS_FAIL;

    if (NULL != mapping_table) {
        bt_sink_srv_state_t state = bt_sink_srv_state_query();
        uint32_t index = 0;

        bt_sink_srv_report("state:0x%x, value:%s, act:%s",
                           state,
                           bt_sink_srv_key_value_strings[key_value],
                           bt_sink_srv_key_action_strings[key_action]);

        while (BT_SINK_SRV_KEY_NONE != mapping_table[index].key_value) {
            if (state & mapping_table[index].sink_state
                    && key_value == mapping_table[index].key_value
                    && key_action == mapping_table[index].key_action) {
                result = bt_sink_srv_action_send(mapping_table[index].sink_action, NULL);
                break;
            }
            index++;
        }
    }
    return result;
}

void bt_sink_srv_init(bt_sink_srv_features_config_t *features)
{
    bt_sink_srv_report("[Sink] bt_sink_srv_init");

    // initialize timer
    bt_sink_srv_timer_init();

    // initialize atci cmd
    bt_sink_srv_atci_init();
    
    bt_device_manager_init();

    bt_sink_srv_config_features(features);

#ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
    bt_sink_srv_audio_setting_init();
#endif
}

