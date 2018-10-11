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

#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_sink_srv_event_callback=_default_bt_sink_srv_event_callback")
#pragma comment(linker, "/alternatename:_bt_sink_srv_get_mapping_table=_default_bt_sink_srv_get_mapping_table")
#pragma comment(linker, "/alternatename:_bt_sink_srv_get_link_loss_reconnect_params=_default_bt_sink_srv_get_link_loss_reconnect_params")
#pragma comment(linker, "/alternatename:_bt_sink_srv_get_power_on_reconnect_params=_default_bt_sink_srv_get_power_on_reconnect_params")
#pragma comment(linker, "/alternatename:_bt_sink_srv_get_visibility_params=_default_bt_sink_srv_get_visibility_params")
#pragma comment(linker, "/alternatename:_bt_sink_srv_get_inquiry_and_connect_aws_params=_default_bt_sink_srv_get_inquiry_and_connect_aws_params")
#pragma comment(linker, "/alternatename:_bt_sink_srv_get_eir=_default_bt_sink_srv_get_eir")
#pragma comment(linker, "/alternatename:_bt_sink_srv_get_ios_hf_features_params=_default_bt_sink_srv_get_ios_hf_features_params")

#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
#pragma weak bt_sink_srv_event_callback = default_bt_sink_srv_event_callback
#pragma weak bt_sink_srv_get_mapping_table = default_bt_sink_srv_get_mapping_table
#pragma weak bt_sink_srv_get_link_loss_reconnect_params = default_bt_sink_srv_get_link_loss_reconnect_params
#pragma weak bt_sink_srv_get_power_on_reconnect_params = default_bt_sink_srv_get_power_on_reconnect_params
#pragma weak bt_sink_srv_get_visibility_params = default_bt_sink_srv_get_visibility_params
#pragma weak bt_sink_srv_get_inquiry_and_connect_aws_params = default_bt_sink_srv_get_inquiry_and_connect_aws_params
#pragma weak bt_sink_srv_get_ios_hf_features_params = default_bt_sink_srv_get_ios_hf_features_params

#pragma weak bt_sink_srv_get_eir = default_bt_sink_srv_get_eir
#else
#error "Unsupported Platform"
#endif


void default_bt_sink_srv_event_callback(bt_sink_srv_event_t event_id, bt_sink_srv_event_param_t *param)
{
    bt_sink_srv_report("[Sink] event_id:%d, param:0x%x", event_id, param);
}

const bt_sink_srv_table_t *default_bt_sink_srv_get_mapping_table(void)
{
    return NULL;
}

const static bt_sink_srv_feature_reconnect_params_t link_loss_reconnect_params = {
    .remote = BT_SINK_SRV_DEVICE_AWS_HEADSET|BT_SINK_SRV_DEVICE_PHONE,
    .attampts = 3
};
const bt_sink_srv_feature_reconnect_params_t *  default_bt_sink_srv_get_link_loss_reconnect_params(void)
{
    return &link_loss_reconnect_params;
}

const static bt_sink_srv_feature_reconnect_params_t power_on_reconnect_params = {
    .remote = BT_SINK_SRV_DEVICE_AWS_HEADSET|BT_SINK_SRV_DEVICE_PHONE,
    .attampts = 3
};
const bt_sink_srv_feature_reconnect_params_t *  default_bt_sink_srv_get_power_on_reconnect_params(void)
{
    return &power_on_reconnect_params;
}

const static bt_sink_srv_feature_visibility_params_t visibility_params = {
    .visibility_duration = 90000,
    .power_on_be_visible_once = false
};
const bt_sink_srv_feature_visibility_params_t * default_bt_sink_srv_get_visibility_params(void)
{
    return &visibility_params;
}

const static bt_sink_srv_feature_inquiry_and_connect_params_t inquiry_and_connect_params = {
    .inquiry_duration = 15,
    .max_count = 20
};
const bt_sink_srv_feature_inquiry_and_connect_params_t* default_bt_sink_srv_get_inquiry_and_connect_aws_params(void)
{
    return &inquiry_and_connect_params;
}

const static uint8_t default_eir_data[16] = {0x1D,0x23,0xBB,0x1B,0x00,0x00,0x10,0x00,0x30,0x00,0x50,0x80,0x5F,0x9B,0x34,0xFA};
const static bt_sink_srv_eir_information_t eir_params = {
    .uuid_128bit = (const uint8_t*)&default_eir_data,
    .rssi = 0    
};

const bt_sink_srv_eir_information_t* default_bt_sink_srv_get_eir(void)
{
    return &eir_params;
}

static bt_sink_srv_ios_hf_features_t default_ios_hf_feature = BT_SINK_SRV_IOS_HF_NONE;
bt_sink_srv_ios_hf_features_t   default_bt_sink_srv_get_ios_hf_features_params(void)
{
    return default_ios_hf_feature;
}

#include "bt_gap.h"
#define __QA_TEST__
// database constants
#define BT_SINK_SRV_CM_DB_NAME  "BT_Headset_Demo"
#define BT_SINK_SRV_CM_DB_COD 0x240408
#define BT_SINK_SRV_CM_DB_IO    BT_GAP_IO_CAPABILITY_NO_INPUT_NO_OUTPUT
#ifdef __QA_TEST__
#include "nvdm.h"
#endif
#ifdef __QA_TEST__
static bt_gap_config_t g_bt_sink_srv_gap_config = {
    .inquiry_mode  = 2, /**< It indicates the inquiry result format.
                                                        0: Standerd inquiry result format (Default).
                                                        1: Inquiry result format with RSSI.
                                                        2: Inquiry result with RSSI format or Extended Inquiry Result(EIR) format. */
    .io_capability = BT_SINK_SRV_CM_DB_IO,
    .cod           = BT_SINK_SRV_CM_DB_COD, /* It defines the class of the local device. */
    .device_name   = {BT_SINK_SRV_CM_DB_NAME}, /* It defines the name of the local device with '\0' ending. */
};
#else
const static bt_gap_config_t g_bt_sink_srv_gap_config = {
    .inquiry_mode  = 2, /**< It indicates the inquiry result format.
                                                        0: Standerd inquiry result format (Default).
                                                        1: Inquiry result format with RSSI.
                                                        2: Inquiry result with RSSI format or Extended Inquiry Result(EIR) format. */
    .io_capability = BT_SINK_SRV_CM_DB_IO,
    .cod           = BT_SINK_SRV_CM_DB_COD, /* It defines the class of the local device. */
    .device_name   = {BT_SINK_SRV_CM_DB_NAME}, /* It defines the name of the local device with '\0' ending. */
};
#endif
const bt_gap_config_t *bt_sink_srv_gap_config(void)
{
#ifdef __QA_TEST__
    uint32_t size = 12;
    uint8_t buffer[18] = {0};
    uint8_t name[30] = "BT_Head_";
    nvdm_status_t status;

    //Change BT local name to BT_Head_xxx for QA test (xxx is BT addr)
    status = nvdm_read_data_item("BT", "address", buffer, &size);
    if (NVDM_STATUS_OK == status) {
        bt_sink_srv_memcpy(name + 8, buffer, (uint32_t)strlen((char *)buffer));
        bt_sink_srv_memcpy(g_bt_sink_srv_gap_config.device_name, name, strlen((char *)name));
        bt_sink_srv_report("[Sink][CM] device name:%s", g_bt_sink_srv_gap_config.device_name);    
    }
#endif
    return &g_bt_sink_srv_gap_config;
}

