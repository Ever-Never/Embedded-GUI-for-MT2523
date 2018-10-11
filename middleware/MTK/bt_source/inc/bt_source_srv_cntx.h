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

#ifndef __BT_SOURCE_SRV_CNTX_H__
#define __BT_SOURCE_SRV_CNTX_H__

#include "stdint.h"
#include "bt_type.h"
#include "FreeRTOS.h"
#include "bt_codec.h"
#include "bt_a2dp.h"
#include "bt_avrcp.h"
#include "bt_source_srv_a2dp.h"
#include "bt_source_srv_avrcp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BT_SOURCE_SRV_MAX_DEVICE_NUMBER          (1)
#define BT_SOURCE_SRV_A2DP_CONN_BIT              (1 << 0)
#define BT_SOURCE_SRV_AVRCP_CONN_BIT             (1 << 1)
#define BT_SOURCE_SRV_INVALID_HD                 (0x00)


typedef enum {
    BT_SOURCE_SRV_DEVICE_A2DP, /* 0 */
    BT_SOURCE_SRV_DEVICE_AVRCP, /* 1 */
    BT_SOURCE_SRV_DEVICE_ADDR, /* 2 */
    BT_SOURCE_SRV_DEVICE_UNUSED, /* 3 */
    BT_SOURCE_SRV_DEVICE_USED, /* 4 */

    BT_SOURCE_SRV_DEVICE_TOTAL
} bt_source_srv_device_type_t;


typedef struct {
    uint32_t timestamp;
    bt_source_srv_a2dp_state_t state;
    bt_media_handle_t *med_handle;
} bt_source_srv_profile_t;

typedef struct {
    bt_source_srv_profile_t profile;
    bt_a2dp_codec_capability_t codec;
} SourceProfileA2dp;

typedef struct {
    bt_bd_addr_t dev_addr;
    uint16_t conn_bit;

    SourceProfileA2dp a2dp;
    uint32_t a2dp_hd;
    bt_a2dp_role_t a2dp_role;
    
    bt_source_srv_avrcp_state_t avrcp_state;
    uint32_t avrcp_hd;
    bt_avrcp_role avrcp_role;
    
    bt_avrcp_media_play_status_event_t avrcp_play_status;
} bt_source_srv_device_t;

typedef struct {  
    bt_source_srv_device_t source_dev[BT_SOURCE_SRV_MAX_DEVICE_NUMBER];
} bt_source_srv_context_t;


void bt_source_srv_cntx_init(void);
bt_source_srv_context_t *bt_source_srv_get_context(void);
bt_source_srv_device_t *bt_source_srv_get_device(bt_source_srv_device_type_t type, const void *param);
void bt_source_srv_reset_device(bt_source_srv_device_t *dev);
void bt_source_srv_reset_a2dp_info(bt_source_srv_device_t *dev);
void bt_source_srv_reset_avrcp_info(bt_source_srv_device_t *dev);


#ifdef __cplusplus
}
#endif

#endif /*__BT_SOURCE_SRV_CNTX_H__*/

