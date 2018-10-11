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

#ifndef __COMMON_DISPATCH_H__
#define __COMMON_DISPATCH_H__

#include "bt_gap.h"
#include "bt_hfp.h"
#include "bt_a2dp.h"
#include "bt_avrcp.h"
#include "bt_gap_le.h"
#include "stdint.h"
#include "stdbool.h"

typedef uint8_t bt_common_callback_mode_t;
#define BQB (0)
#define TF (1)
#define CLM (2)
#define SINK (3)

bool bt_common_callback_set_mode(bt_common_callback_mode_t mode);

bt_common_callback_mode_t bt_common_callback_get_mode();

#define __bt_app_event_callback__
#ifdef __bt_app_event_callback__
#define _TF_bt_app_event_callback TF_bt_app_event_callback
#define _CLM_bt_app_event_callback CLM_bt_app_event_callback 
#define _SINK_bt_app_event_callback SINK_bt_app_event_callback
#endif

#define __bt_gap_le_get_local_config__
#ifdef __bt_gap_le_get_local_config__
#define _TF_bt_gap_le_get_local_config TF_bt_gap_le_get_local_config
#define _CLM_bt_gap_le_get_local_config CLM_bt_gap_le_get_local_config 
#define _SINK_bt_gap_le_get_local_config SINK_bt_gap_le_get_local_config
#endif

#define __bt_gap_le_get_bonding_info__
#ifdef __bt_gap_le_get_bonding_info__
#define _TF_bt_gap_le_get_bonding_info TF_bt_gap_le_get_bonding_info
#define _CLM_bt_gap_le_get_bonding_info CLM_bt_gap_le_get_bonding_info
#define _SINK_bt_gap_le_get_bonding_info SINK_bt_gap_le_get_bonding_info
#endif


#define __bt_gap_le_get_pairing_config__
#ifdef __bt_gap_le_get_pairing_config__
#define _TF_bt_gap_le_get_pairing_config TF_bt_gap_le_get_pairing_config
#define _CLM_bt_gap_le_get_pairing_config CLM_bt_gap_le_get_pairing_config 
#endif

#define __bt_gatts_get_authorization_check_result__
#ifdef __bt_gatts_get_authorization_check_result__
#define _TF_bt_gatts_get_authorization_check_result TF_bt_gatts_get_authorization_check_result
#define _CLM_bt_gatts_get_authorization_check_result CLM_bt_gatts_get_authorization_check_result
#endif

#define __bt_gatts_get_execute_write_result__
#ifdef __bt_gatts_get_execute_write_result__
#define _TF_bt_gatts_get_execute_write_result TF_bt_gatts_get_execute_write_result
#define _CLM_bt_gatts_get_execute_write_result CLM_bt_gatts_get_execute_write_result
#endif

#define __bt_gap_get_link_key__
#ifdef __bt_gap_get_link_key__
#define _SINK_bt_gap_get_link_key SINK_bt_gap_get_link_key
#endif

#define __bt_a2dp_get_init_params__
#ifdef __bt_a2dp_get_init_params__
#define _CLM_bt_a2dp_get_init_params CLM_bt_a2dp_get_init_params
#define _SINK_bt_a2dp_get_init_params SINK_bt_a2dp_get_init_params
#endif

#define __bt_hfp_get_init_params__
#ifdef __bt_hfp_get_init_params__
#define _CLM_bt_hfp_get_init_params CLM_bt_hfp_get_init_params
#define _SINK_bt_hfp_get_init_params SINK_bt_hfp_get_init_params
#endif


#define __bt_config_callback__

#ifdef __bt_config_callback__
#endif

#endif /*__COMMON_DISPATCH_H__*/




