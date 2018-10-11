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
#include "bt_sink_srv_resource.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_conmgr.h"
#include "bt_gap_le.h"
#include "bt_system.h"
#include "bt_sink_srv_hf.h"
#include "bt_sink_srv_a2dp.h"
#include "bt_sink_srv_avrcp.h"
#include "bt_sink_srv_pbapc.h"
#include "bt_sink_srv_spp.h"
#include "bt_sink_srv_ami.h"
#ifdef __BT_AWS_SUPPORT__
#include "bt_sink_srv_aws.h"
#endif

bt_sink_srv_features_config_t bt_sink_srv_features_config;

void bt_sink_srv_config_features(bt_sink_srv_features_config_t *features)
{
    bt_sink_srv_memcpy(&bt_sink_srv_features_config, features, sizeof(bt_sink_srv_features_config_t));
}

const bt_sink_srv_features_config_t *bt_sink_srv_get_features_config(void)
{
    return &bt_sink_srv_features_config;
}


bt_status_t bt_sink_srv_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_status_t result = BT_STATUS_SUCCESS;
    uint32_t moduel = msg & 0xFF000000;

    switch (moduel) {
        case BT_MODULE_SYSTEM:
            result = bt_sink_srv_cm_system_callback(msg, status, buffer);
            break;

        case BT_MODULE_GAP:
            result = bt_sink_srv_cm_gap_callback(msg, status, buffer);
            break;

        case BT_MODULE_HFP:
            result = bt_sink_srv_hf_common_callback(msg, status, buffer);
            break;

        case BT_MODULE_A2DP:
            result = bt_sink_srv_a2dp_common_callback(msg, status, buffer);
            break;

        case BT_MODULE_AVRCP:
            result = bt_sink_srv_avrcp_common_callback(msg, status, buffer);
            break;

        case BT_MODULE_PBAPC:
            result = bt_sink_srv_pbapc_common_callback(msg, status, buffer);
            break;

        case BT_MODULE_SPP:
            //result = bt_sink_srv_spp_common_callback(msg, status, buffer);
            break;

        #ifdef __BT_AWS_SUPPORT__
        case BT_MODULE_AWS:
            result = bt_sink_srv_aws_common_callback(msg, status, buffer);
            break;
        #endif

        default:
            bt_sink_srv_report("[Sink] Unknown Bluetooth MSG:0x%x, status:0x%x", msg, status);
            break;
    }
    return result;
}

