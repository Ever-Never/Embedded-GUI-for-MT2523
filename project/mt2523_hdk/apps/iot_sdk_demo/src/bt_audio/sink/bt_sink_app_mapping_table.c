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

#include "FreeRTOS.h"
#include "task.h"
#include "bt_sink_srv.h"

static const bt_sink_srv_table_t g_bt_sink_app_mapping_table[] = {
    // Call control
    {
        // ANSWER INCOMING
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_INCOMING,
        BT_SINK_SRV_ACTION_ANSWER
    },
    {
        // REJECT INCOMING
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_INCOMING,
        BT_SINK_SRV_ACTION_REJECT
    },
    {
        // HANG UP ACTIVE/OUTGOING
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        (bt_sink_srv_state_t)(BT_SINK_SRV_STATE_ACTIVE | BT_SINK_SRV_STATE_OUTGOING |
        BT_SINK_SRV_STATE_HELD_ACTIVE | BT_SINK_SRV_STATE_TWC_INCOMING |
        BT_SINK_SRV_STATE_MULTIPARTY),
        BT_SINK_SRV_ACTION_HANG_UP
    },
    {
        // HOLD ACTIVE
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_ACTIVE,
        BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER
    },
    {
        // RETRIEVE HELD
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_HELD_REMAINING,
        BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER
    },
    {
        // SWAP ACTIVE AND HELD
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_HELD_ACTIVE,
        BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER
    },
    {
        // HOLD AND ACCEPT
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_TWC_INCOMING,
        BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER
    },
    {
        // REJECT WAITING
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK,
        BT_SINK_SRV_STATE_TWC_INCOMING,
        BT_SINK_SRV_ACTION_3WAY_RELEASE_ALL_HELD
    },
    {
        // REDIAL LAST DIALED NUMBER
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        BT_SINK_SRV_STATE_CONNECTED,
        BT_SINK_SRV_ACTION_DIAL_LAST
    },
    {
        // SWITCH AUDIO PATH
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK,
        (bt_sink_srv_state_t)(BT_SINK_SRV_STATE_ACTIVE | BT_SINK_SRV_STATE_HELD_ACTIVE),
        BT_SINK_SRV_ACTION_SWITCH_AUDIO_PATH
    },
    // Remote volume control
    {
        BT_SINK_SRV_KEY_NEXT,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        (bt_sink_srv_state_t)(BT_SINK_SRV_STATE_ACTIVE | BT_SINK_SRV_STATE_HELD_ACTIVE),
        BT_SINK_SRV_ACTION_VOLUME_UP
    },
    {
        BT_SINK_SRV_KEY_PREV,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        (bt_sink_srv_state_t)(BT_SINK_SRV_STATE_ACTIVE | BT_SINK_SRV_STATE_HELD_ACTIVE),
        BT_SINK_SRV_ACTION_VOLUME_DOWN
    },
    {
        BT_SINK_SRV_KEY_PREV,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        (bt_sink_srv_state_t)(BT_SINK_SRV_STATE_ACTIVE | BT_SINK_SRV_STATE_HELD_ACTIVE),
        BT_SINK_SRV_ACTION_VOLUME_MIN
    },
    {
        BT_SINK_SRV_KEY_NEXT,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        (bt_sink_srv_state_t)(BT_SINK_SRV_STATE_ACTIVE | BT_SINK_SRV_STATE_HELD_ACTIVE),
        BT_SINK_SRV_ACTION_VOLUME_MAX
    },
    // Music control
    {
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_STREAMING,
        BT_SINK_SRV_ACTION_PAUSE
    },
    {
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_PRESS_UP,
        BT_SINK_SRV_STATE_CONNECTED,
        BT_SINK_SRV_ACTION_PLAY
    },
    {
        BT_SINK_SRV_KEY_NEXT,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        (bt_sink_srv_state_t)(BT_SINK_SRV_STATE_CONNECTED | BT_SINK_SRV_STATE_STREAMING),
        BT_SINK_SRV_ACTION_NEXT_TRACK
    },
    {
        BT_SINK_SRV_KEY_PREV,
        BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP,
        (bt_sink_srv_state_t)(BT_SINK_SRV_STATE_CONNECTED | BT_SINK_SRV_STATE_STREAMING),
        BT_SINK_SRV_ACTION_PRE_TRACK
    },
    /* CONNECTION MANAGEMENT */
    {
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK,
        BT_SINK_SRV_STATE_POWER_ON,
        BT_SINK_SRV_ACTION_CONNECT
    },
    {
        BT_SINK_SRV_KEY_FUNC,
        BT_SINK_SRV_KEY_ACT_LONG_LONG_PRESS_DOWN,
        BT_SINK_SRV_STATE_POWER_ON,
        BT_SINK_SRV_ACTION_DISCOVERABLE
    },

    /* Add before this line */
    {
        BT_SINK_SRV_KEY_NONE,
        BT_SINK_SRV_KEY_ACT_NONE,
        BT_SINK_SRV_STATE_NONE,
        BT_SINK_SRV_ACTION_ALL
    }
};

const bt_sink_srv_table_t *bt_sink_srv_get_mapping_table(void)
{
    return g_bt_sink_app_mapping_table;
}
