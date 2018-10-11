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

#include <string.h>
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv.h"
#include "bt_system.h"
#include "bt_sink_srv_spp.h"

bt_sink_srv_spp_context_t *bt_sink_srv_spp_device = NULL;

void bt_sink_srv_spp_disconnect(bt_bd_addr_t *addr)
{
    if (NULL != bt_sink_srv_spp_device && 0 != bt_sink_srv_spp_device->spp_handle
            && 0 == bt_sink_srv_memcmp((void *)addr, (void *)&bt_sink_srv_spp_device->bt_addr, sizeof(bt_bd_addr_t))) {
        bt_spp_disconnect(bt_sink_srv_spp_device->spp_handle);
    }
}

bt_sink_srv_status_t bt_sink_srv_spp_action_handler(bt_sink_srv_action_t action, void *parameters)
{
    switch (action) {
        case BT_SINK_SRV_ACTION_PROFILE_DISCONNECT: {
            //bt_sink_srv_profile_connection_action_t *action_param = (bt_sink_srv_profile_connection_action_t *)parameters;
            if (NULL != bt_sink_srv_spp_device && 0 != bt_sink_srv_spp_device->spp_handle) {
                bt_spp_disconnect(bt_sink_srv_spp_device->spp_handle);
            }
        }
        break;

        default:
            break;
    }
    return BT_SINK_SRV_STATUS_SUCCESS;
}

bt_status_t bt_sink_srv_spp_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_sink_srv_report("[Sink][SPP] SDK msg:0x%x", msg);

    switch (msg) {
        case BT_SPP_CONNECT_IND: {
            bt_spp_connect_ind_t *message = (bt_spp_connect_ind_t *)buffer;

            bt_sink_srv_spp_device = (bt_sink_srv_spp_context_t *)bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_spp_context_t));
            bt_sink_srv_memcpy((void *)&bt_sink_srv_spp_device->bt_addr,
                               (void *)message->address,
                               sizeof(bt_bd_addr_t));
            bt_sink_srv_spp_device->spp_handle = message->handle;
        }
        break;

        case BT_SPP_CONNECT_CNF: {
            if (BT_STATUS_SUCCESS == status) {
                bt_sink_srv_action_send(BT_SINK_SRV_ACTION_CONNECT, bt_sink_srv_spp_device);
            } else {
                bt_sink_srv_memory_free(bt_sink_srv_spp_device);
                bt_sink_srv_spp_device = NULL;
            }
        }
        break;

        case BT_SPP_DISCONNECT_IND: {
            bt_sink_srv_memory_free(bt_sink_srv_spp_device);
            bt_sink_srv_spp_device = NULL;
        }
        break;

        default:
            break;
    }
    return BT_STATUS_SUCCESS;
}

