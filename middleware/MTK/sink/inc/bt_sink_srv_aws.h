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
#ifndef __BT_SINK_SRV_AWS_H__
#define __BT_SINK_SRV_AWS_H__
#include "bt_aws.h"
#include "bt_sink_srv.h"
#ifdef __BT_AWS_SUPPORT__

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t bt_aws_role_t;
#define BT_AWS_ROLE_SOURCE      0x00                      /**< The AWS role of source. */
#define BT_AWS_ROLE_SINK        0x01                      /**< The AWS role of sink. */
#define BT_AWS_ROLE_NONE        0xFF                      /**< The AWS role of none. */

typedef uint16_t bt_sink_srv_cmd_id_t;
#define BT_SINK_SRV_AWS_COMMAND_ID_START       0x0001
#define BT_SINK_SRV_AWS_COMMAND_ID_STOP        0X0002
#define BT_SINK_SRV_AWS_COMMAND_ID_NEXT        0X0003
#define BT_SINK_SRV_AWS_COMMAND_ID_PREV        0X0004
#define BT_SINK_SRV_AWS_COMMAND_ID_SET_VOLUME  0X0005
#define BT_SINK_SRV_AWS_COMMAND_ID_SET_ROLE    0X0006
#define BT_SINK_SRV_AWS_COMMAND_ID_SET_MODE    0X0007
#define BT_SINK_SRV_AWS_COMMAND_ID_SYSTEM_POWER_OFF    0X0008
#define BT_SINK_SRV_AWS_COMMAND_ID_SYSTEM_KEY    0X0009
#define BT_SINK_SRV_AWS_COMMAND_ID_SYSTEM_SINK_STATE    0X000A
#define BT_SINK_SRV_AWS_COMMAND_ID_SYSTEM_SINK_ACTION    0X000B
#define BT_SINK_SRV_AWS_COMMAND_ID_SINK_PLAY        0x000C
#define BT_SINK_SRV_AWS_COMMAND_ID_SINK_PAUSE       0x000D
#define BT_SINK_SRV_AWS_COMMAND_ID_SYNC_BASE       0x000E

#define BT_SINK_SRV_AWS_COMMAND_END            0X000F

typedef uint8_t bt_sink_srv_cmd_type_t;
#define BT_SINK_SRV_AWS_COMMAND_TYPE_REQ       0X01
#define BT_SINK_SRV_AWS_COMMAND_TYPE_RSP       0X02
#define BT_SINK_SRV_AWS_COMMAND_TYPE_CUSTOMIZE 0X03

#define BT_SINK_SRV_AWS_MODULE_ID_A2DP   0
#define BT_SINK_SRV_AWS_MODULE_ID_MP3    1
#define BT_SINK_SRV_AWS_MODULE_ID_CALL   2
#define BT_SINK_SRV_AWS_MODULE_ID_SYSTEM 3

typedef uint32_t bt_aws_codec_type_t;
#define BT_AWS_CODEC_TYPE_SBC        (0x00)           /**< SBC codec. */
#define BT_AWS_CODEC_TYPE_MP3        (0x01)           /**< MP3 codec. */
#define BT_AWS_CODEC_TYPE_AAC        (0x02)           /**< AAC codec. */

typedef uint8_t bt_sink_srv_aws_state_t;
#define BT_SINK_SRV_AWS_STATE_NONE  0x01
#define BT_SINK_SRV_AWS_STATE_READY 0x02
#define BT_SINK_SRV_AWS_STATE_CONNECTED 0x03


typedef struct {
    uint32_t aws_handle;
    bt_sink_srv_aws_state_t state;
} bt_sink_srv_aws_context_t;

/**
 * @brief This structure defines mp3 codec needed parameters
 */
typedef struct {
    uint8_t change_track;
} bt_mp3_codec_t;

/**
 *  @brief This structure defines the AWS codec capability.
 */
typedef struct {
  bt_aws_codec_type_t type;      /**< Codec type. */
  uint8_t   length;              /**< The length of the following codec. */
  union {
        bt_a2dp_aac_codec_t aac; /**< AAC codec. */
        bt_a2dp_sbc_codec_t sbc; /**< SBC codec. */
        bt_mp3_codec_t mp3;
  } codec;                       /**< Codec information. */
} bt_aws_codec_capability_t;

typedef struct
{
    bt_sink_srv_cmd_id_t cmd_id;
    bt_sink_srv_cmd_type_t type;
    uint8_t accept;
    uint16_t data_len;
    uint8_t data[1];
} bt_sink_srv_aws_cmd_t;

typedef struct
{
    bt_sink_srv_cmd_id_t cmd_id;
    uint16_t data_len;
    uint8_t *data;
} bt_sink_srv_aws_cmd_ind_t;

typedef struct
{
    bt_sink_srv_cmd_id_t cmd_id;
    uint8_t accept;
    uint16_t data_len;
    uint8_t *data;
} bt_sink_srv_aws_cmd_rsp_t;


typedef struct
{
    uint32_t handle;
    bt_sink_srv_cmd_type_t type;
    union
    {
        bt_sink_srv_aws_cmd_ind_t cmd_ind;
        bt_sink_srv_aws_cmd_rsp_t cmd_rsp;
        bt_aws_command_ind_t cmd;
    };
} bt_aws_command_result_t;

typedef bt_status_t  (*aws_callback_func_t)(bt_msg_type_t msg, bt_status_t status, void *buffer);


typedef struct
{
    bt_aws_module_id_t module_id;
    aws_callback_func_t aws_callback;
}bt_sink_srv_aws_cb_t;

bt_status_t bt_sink_srv_aws_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);

bt_status_t bt_sink_srv_aws_send_cmd(
                                uint32_t handle,
                                bt_aws_module_id_t module_id,
                                bt_sink_srv_aws_cmd_t *aws_cmd);

bt_sink_srv_status_t bt_sink_srv_aws_action_handler(bt_sink_srv_action_t action, void *parameters);

bt_bd_addr_t *bt_sink_srv_aws_get_address_by_handle(uint32_t handle);


#ifdef __cplusplus
}
#endif


#endif /* __BT_AWS_SUPPORT__ */
#endif /* __BT_SINK_SRV_AWS_H__ */

