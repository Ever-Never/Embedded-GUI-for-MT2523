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

#include "bt_sink_srv_avrcp.h"
#include "bt_sink_srv_music.h"
#include "bt_sink_srv_action.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_state_notify.h"
#include "bt_sink_srv.h"
#include "bt_sink_srv_resource.h"
#include "audio_src_srv.h"
#include "audio_src_srv_internal.h"


#define AUD_VOL_IN_LEVEL_VALUE_INTERVAL (9)
#define AUD_VOL_IN_LEVEL_VALUE_MAX (127)

static bool vol_rsp_flag = false;

/* Function declare */
static void bt_sink_srv_avrcp_init();

static int32_t bt_sink_srv_avrcp_handle_connect_cnf(bt_avrcp_connect_cnf_t *conn_cnf, bt_status_t status);

static int32_t bt_sink_srv_avrcp_handle_connect_ind(bt_avrcp_connect_ind_t *conn_ind, bt_status_t status);

static int32_t bt_sink_srv_avrcp_handle_disconnect_ind(bt_avrcp_disconnect_ind_t *disconn_ind, bt_status_t status);

static int32_t bt_sink_srv_avrcp_handle_pass_through_cnf(bt_avrcp_pass_through_cnf_t *through_cnf, bt_status_t status);

static int32_t bt_sink_srv_avrcp_handle_event_notification_ind(bt_avrcp_event_notification_t *noti_ind, bt_status_t status);

static int32_t bt_sink_srv_avrcp_handle_register_notification_ind(bt_avrcp_register_notification_commant_t *register_ind, bt_status_t status);

static int32_t bt_sink_srv_avrcp_handle_set_absolute_volume_ind(bt_avrcp_set_absolute_volume_event_t *vol_ind, bt_status_t status);

extern int32_t bt_sink_srv_a2dp_set_volume(uint8_t volume);



static void bt_sink_srv_avrcp_init()
{
    ;
}


uint8_t bt_sink_srv_avrcp_get_volume_level(uint8_t volume)
{
    uint8_t vol_level = AUD_VOL_OUT_LEVEL0;

    if (0 == volume) {
        return vol_level;
    }
    vol_level = (volume/AUD_VOL_IN_LEVEL_VALUE_INTERVAL) + 1;
    bt_sink_srv_report("[sink][avrcp]get_volume_level[s]-vlume: %d,volume_level: %d", volume, vol_level);
    return vol_level;
}


uint8_t bt_sink_srv_avrcp_get_volume_value(uint8_t vol_level)
{
    uint8_t volume = 0;

    bt_sink_srv_report("[sink][avrcp]get_volume_value[s]-volume_level: %d", vol_level);
    //if ((AUD_VOL_OUT_LEVEL0 <= (bt_sink_srv_am_volume_level_out_t)vol_level) && (AUD_VOL_OUT_LEVEL14 >= (bt_sink_srv_am_volume_level_out_t)vol_level)) {
    if ((AUD_VOL_OUT_LEVEL14 >= (bt_sink_srv_am_volume_level_out_t)vol_level)) {
        volume = vol_level * AUD_VOL_IN_LEVEL_VALUE_INTERVAL;
    } else if (AUD_VOL_OUT_LEVEL15 == (bt_sink_srv_am_volume_level_out_t)vol_level) {
        volume = AUD_VOL_IN_LEVEL_VALUE_MAX;
    }

    bt_sink_srv_report("[sink][avrcp]get_volume_value[e]-volume_value: %d", volume);
    return volume;
}


bt_status_t bt_sink_srv_avrcp_volume_notification(uint32_t handle, uint8_t vol_level)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    uint8_t volume = 0;

    if (vol_rsp_flag) {
        bt_avrcp_send_register_notification_response_t rsp;
        volume = bt_sink_srv_avrcp_get_volume_value(vol_level);
        rsp.event_id = BT_AVRCP_EVENT_VOLUME_CHANGED;
        rsp.parameter_length = 2;
        rsp.response_type = BT_AVRCP_RESPONSE_CHANGED;
        rsp.volume = volume;
        bt_sink_srv_report("[sink][avrcp]vol_noti[s]-volume_value: %d", volume);
        vol_rsp_flag = false;
        ret = bt_avrcp_send_register_notification_response(handle, &rsp);
    }

    bt_sink_srv_report("[sink][avrcp]vol_noti[e]-volume_value: %d, ret: %d", volume, ret);

    return ret;
}


static int32_t bt_sink_srv_avrcp_handle_connect_cnf(bt_avrcp_connect_cnf_t *conn_cnf, bt_status_t status)
{
    bt_sink_srv_music_device_t *dev = NULL;
    bt_bd_addr_t dev_addr = {0};

    bt_sink_srv_report("[sink][avrcp]connect_cnf-avrcp_hd: 0x%x", conn_cnf->handle);

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AVRCP_HD, (void *)(&(conn_cnf->handle)));
    if (dev) {
        memcpy(&dev_addr, &(dev->dev_addr), sizeof(bt_bd_addr_t));
        if (BT_STATUS_SUCCESS == status) {
            BT_SINK_SRV_SET_FLAG(dev->conn_bit, BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT);
            bt_avrcp_register_notification(conn_cnf->handle, BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED, 20);  
        }
        bt_sink_srv_cm_profile_status_notify(&dev_addr, BT_SINK_SRV_PROFILE_AVRCP, BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED);
    } else {
        bt_sink_srv_report("[sink][avrcp]connect_cnf(error)-avrcp_hd: 0x%x", conn_cnf->handle);
        if (status == BT_STATUS_SUCCESS) {
            bt_avrcp_disconnect(conn_cnf->handle);
        }
    }

    return BT_STATUS_SUCCESS;
}


static int32_t bt_sink_srv_avrcp_handle_connect_ind(bt_avrcp_connect_ind_t *conn_ind, bt_status_t status)
{
    bt_sink_srv_music_device_t *dev = NULL;
    int32_t ret = 0;

    bt_sink_srv_report("[sink][avrcp]connect_ind-avrcp_hd: 0x%x", conn_ind->handle);

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_A2DP, conn_ind->address);
    if (dev) {
        ;
    } else {
        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_UNUSED, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP);
        bt_sink_srv_memcpy(&(dev->dev_addr), conn_ind->address, sizeof(bt_bd_addr_t));
    }
    bt_sink_srv_assert(dev);
    dev->avrcp_hd = conn_ind->handle;
    ret = bt_avrcp_connect_response(conn_ind->handle, true);

    return ret;
}


static int32_t bt_sink_srv_avrcp_handle_disconnect_ind(bt_avrcp_disconnect_ind_t *disconn_ind, bt_status_t status)
{
    bt_sink_srv_music_device_t *dev = NULL;
    int32_t ret = 0;
    bt_bd_addr_t dev_addr = {0};

    bt_sink_srv_report("[sink][avrcp]disconnect_ind-avrcp_hd: 0x%x", disconn_ind->handle);

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AVRCP_HD, (void *)(&(disconn_ind->handle)));

    if (dev) {
        memcpy(&dev_addr, &(dev->dev_addr), sizeof(bt_bd_addr_t));
        BT_SINK_SRV_REMOVE_FLAG(dev->conn_bit, BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT);
        if (!(dev->conn_bit)) {
            bt_sink_srv_music_reset_device(dev);
        }
        bt_sink_srv_cm_profile_status_notify(&dev_addr, BT_SINK_SRV_PROFILE_AVRCP, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED);
    }

    return ret;
}


static int32_t bt_sink_srv_avrcp_handle_pass_through_cnf(bt_avrcp_pass_through_cnf_t *through_cnf, bt_status_t status)
{
    int32_t ret = 0;

    if (status == BT_STATUS_SUCCESS) {
        if (through_cnf->op_state == 0) {
            ret = bt_avrcp_send_pass_through_command(through_cnf->handle, through_cnf->op_id, 1);
        } else if (through_cnf->op_state == 1) {
            /* ignore */
        }
    }

    return ret;
}


static int32_t bt_sink_srv_avrcp_handle_event_notification_ind(bt_avrcp_event_notification_t *noti_ind, bt_status_t status)
{
    int32_t ret = 0;
    bt_sink_srv_music_device_t *dev = NULL, *aws_dev = NULL;
#ifdef __BT_SINK_SRV_VOICE_PROMPT__
    audio_src_srv_context_t *audio_src_ctx = audio_src_srv_get_ctx();
#endif

    if (BT_STATUS_SUCCESS != status && BT_STATUS_AVRCP_INTERIM != status) {
        bt_sink_srv_report("[sink][avrcp]event_notification_ind(fail)--evt_id: %d, reason: 0x%08x", noti_ind->event_id, status);
    } else {
        if (BT_STATUS_AVRCP_INTERIM == status) {
            bt_sink_srv_report("[sink][avrcp]event_notification_ind(interim)--evt_id: %d, status: %d", noti_ind->event_id, noti_ind->status);
            if (noti_ind && noti_ind->event_id == BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED) {
                dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AVRCP_HD, (void *)(&(noti_ind->handle)));
                aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
                bt_sink_srv_assert(dev);
                dev->avrcp_status = noti_ind->status;

                if ((dev->avrcp_status == 0x02) &&
                    (dev->op & BT_SINK_SRV_MUSIC_A2DP_HF_INTERRUPT)) {
                    BT_SINK_SRV_REMOVE_FLAG(dev->op, BT_SINK_SRV_MUSIC_A2DP_HF_INTERRUPT);
                    audio_src_srv_del_waiting_list(dev->handle);
                }

                if ((dev->avrcp_status == 0x00) &&
                    (dev->op & BT_SINK_SRV_MUSIC_A2DP_HF_INTERRUPT)) {
                    BT_SINK_SRV_REMOVE_FLAG(dev->op, BT_SINK_SRV_MUSIC_A2DP_HF_INTERRUPT);
                    audio_src_srv_del_waiting_list(dev->handle);
                }
            #ifdef __BT_SINK_SRV_AVRCP_STATUS_SUPPORT__
                /* AWS conn */
                if (noti_ind->status == 0x01 && dev && !aws_dev) { /* Play status playing */
                    if (!(dev->op & BT_SINK_SRV_MUSIC_OP_CODEC_OPEN)) {
                    #ifdef __BT_SINK_SRV_VOICE_PROMPT__
                        if (audio_src_ctx->running && audio_src_ctx->running->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_VP) {
                            /* Ignore it & Wait A2DP start ind handle */
                            ;
                        } else {
                            BT_SINK_SRV_SET_FLAG(dev->op, BT_SINK_SRV_MUSIC_AVRCP_PLAY_TRIGGER);
                            bt_sink_srv_music_state_machine_handle(dev, BT_SINK_SRV_MUSIC_EVT_AVRCP_PLAYING, NULL);
                            BT_SINK_SRV_REMOVE_FLAG(dev->op, BT_SINK_SRV_MUSIC_AVRCP_PLAY_TRIGGER);
                        }
                    #else
                        BT_SINK_SRV_SET_FLAG(dev->op, BT_SINK_SRV_MUSIC_AVRCP_PLAY_TRIGGER);
                        bt_sink_srv_music_state_machine_handle(dev, BT_SINK_SRV_MUSIC_EVT_AVRCP_PLAYING, NULL);
                        BT_SINK_SRV_REMOVE_FLAG(dev->op, BT_SINK_SRV_MUSIC_AVRCP_PLAY_TRIGGER);
                    #endif
                    } else {
                        bt_sink_srv_state_set(BT_SINK_SRV_STATE_STREAMING);
                    }
                } else if (noti_ind->status == 0x02 && dev && !aws_dev
                    && (dev->op & BT_SINK_SRV_MUSIC_OP_CODEC_OPEN)) { /* Play status paused */
                    //bt_sink_srv_music_state_machine_handle(dev, BT_SINK_SRV_MUSIC_EVT_AVRCP_PAUSED, NULL);
                    bt_sink_srv_state_reset(BT_SINK_SRV_STATE_STREAMING);
                } else {
                    bt_sink_srv_report("[sink][avrcp]event_notification_ind(error)--flag: 0x%08x, op: 0x%08x", dev->flag, dev->op);
                }
            #endif /* __BT_SINK_SRV_AVRCP_STATUS_SUPPORT__ */
            }
        } else {
            if (noti_ind && noti_ind->event_id == BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED) {
                bt_sink_srv_report("[sink][avrcp]event_notification_ind(changed)--evt_id: %d, status: %d", noti_ind->event_id, noti_ind->status);
                ret = bt_avrcp_register_notification(noti_ind->handle, BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED, 20);
            }
        }
    }

    return ret;
}

static int32_t bt_sink_srv_avrcp_handle_register_notification_ind(bt_avrcp_register_notification_commant_t *register_ind, bt_status_t status)
{
    int32_t ret = 0;

    if (BT_STATUS_SUCCESS == status) {
        if (BT_AVRCP_EVENT_VOLUME_CHANGED == register_ind->event_id) {
            bt_avrcp_send_register_notification_response_t rsp;
            bt_sink_srv_music_context_t *cntx = bt_sink_srv_music_get_context();

            vol_rsp_flag = true;
            rsp.event_id = BT_AVRCP_EVENT_VOLUME_CHANGED;
            rsp.parameter_length = 2;
            rsp.response_type = BT_AVRCP_RESPONSE_INTERIM;
            rsp.volume = bt_sink_srv_avrcp_get_volume_value(cntx->vol_lev);

            bt_sink_srv_report("[sink][avrcp]register_notificatin_ind[s]-volume_value: %d", rsp.volume);
            //todo to get the local volume and update volume, when local volume update;
            ret = bt_avrcp_send_register_notification_response(register_ind->handle, &rsp);
        }
    }

    return ret;
}


static int32_t bt_sink_srv_avrcp_handle_set_absolute_volume_ind(bt_avrcp_set_absolute_volume_event_t *vol_ind, bt_status_t status)
{
    int32_t ret = 0;

    if (BT_STATUS_SUCCESS == status) {
        uint8_t volume = vol_ind->volume;
        bt_sink_srv_a2dp_set_volume(volume);

        bt_sink_srv_report("[sink][avrcp]absolute_volume_ind[s]-volume_value: %d", volume);
        ret = bt_avrcp_send_set_absoulte_volume_response(vol_ind->handle, volume);
    }
    return ret;
}


bt_sink_srv_status_t bt_sink_srv_avrcp_action_handler(bt_sink_srv_action_t action, void *param)
{
    bt_status_t ret = 0;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_profile_connection_action_t *conn_info = NULL;
    bt_bd_addr_t *dev_addr = NULL;
    int32_t err = 0;

    bt_sink_srv_report("[sink][avrcp]process_avrcp_action[s]-event: 0x%x, base: 0x%x", action, BT_SINK_SRV_ACTION_AVRCP_START);

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);

    if ((dev) && !(dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
        /* Find another A2DP & AVRCP connection device */
        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_SP, NULL);
    }

#ifdef __BT_SINK_SRV_MUSIC_MODE_SUPPORT__
    if (bt_sink_srv_get_music_mode() == BT_SINK_SRV_MODE_LOCAL_MUSIC) {
        if ((action == BT_SINK_SRV_ACTION_PLAY) ||
            (action == BT_SINK_SRV_ACTION_PAUSE) ||
            (action == BT_SINK_SRV_ACTION_NEXT_TRACK) ||
            (action == BT_SINK_SRV_ACTION_PRE_TRACK)) {
            /* Ignore in local mode */
            bt_sink_srv_report("[sink][avrcp]process_avrcp_action-Ignore");
            return BT_SINK_SRV_STATUS_SUCCESS;
        }
    }
#endif /* __BT_SINK_SRV_MUSIC_MODE_SUPPORT__ */

    switch (action) {
        case BT_SINK_SRV_ACTION_PROFILE_INIT: {
            bt_sink_srv_avrcp_init();
            break;
        }

        case BT_SINK_SRV_ACTION_PROFILE_DISCONNECT: {
            conn_info = (bt_sink_srv_profile_connection_action_t *)param;
            dev_addr = &(conn_info->address);
            if (conn_info->profile_connection_mask & BT_SINK_SRV_PROFILE_AVRCP) {
                dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_A2DP, dev_addr);
                if ((dev) && (dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
                    ret = bt_avrcp_disconnect(dev->avrcp_hd);
                } else {
                    err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
                }
            }
            break;
        }

        case BT_SINK_SRV_ACTION_PLAY: {
            if (dev) {
                ret = bt_sink_srv_avrcp_play_music(dev->avrcp_hd);
            }
            break;
        }

        case BT_SINK_SRV_ACTION_PAUSE: {
            if (dev) {
                ret = bt_sink_srv_avrcp_stop_music(dev->avrcp_hd);
            }
            break;
        }

        case BT_SINK_SRV_ACTION_NEXT_TRACK: {
            if (dev) {
                ret = bt_sink_srv_avrcp_change_ntrack(dev->avrcp_hd);
            }
            break;
        }

        case BT_SINK_SRV_ACTION_PRE_TRACK: {
            if (dev) {
                ret = bt_sink_srv_avrcp_change_ptrack(dev->avrcp_hd);
            }
            break;
        }

        default:
            break;
    }

    bt_sink_srv_report("[sink][avrcp]process_avrcp_action[e]-ret: %d, err: %d", ret, err);

    return BT_SINK_SRV_STATUS_SUCCESS;
}


bt_status_t bt_sink_srv_avrcp_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    int32_t ret = 0;
    // TODO:
    //SinkProfileAvrcp *avrcp = NULL;

    bt_sink_srv_report("[sink][avrcp]common_hdr[s]-msg: 0x%x, status: %d", msg, status);

    switch (msg) {
        case BT_AVRCP_CONNECT_CNF: {
            bt_avrcp_connect_cnf_t *conn_cnf = (bt_avrcp_connect_cnf_t *)buffer;
            ret = bt_sink_srv_avrcp_handle_connect_cnf(conn_cnf, status);
            break;
        }

        case BT_AVRCP_CONNECT_IND: {
            bt_avrcp_connect_ind_t *conn_ind = (bt_avrcp_connect_ind_t *)buffer;
            ret = bt_sink_srv_avrcp_handle_connect_ind(conn_ind, status);
            break;
        }

        case BT_AVRCP_DISCONNECT_IND: {
            bt_avrcp_disconnect_ind_t *disconn_ind = (bt_avrcp_disconnect_ind_t *)buffer;
            ret = bt_sink_srv_avrcp_handle_disconnect_ind(disconn_ind, status);
            break;
        }

        case BT_AVRCP_PASS_THROUGH_CNF: {
            bt_avrcp_pass_through_cnf_t *through_cnf = (bt_avrcp_pass_through_cnf_t *)buffer;
            ret = bt_sink_srv_avrcp_handle_pass_through_cnf(through_cnf, status);
            break;
        }

        case BT_AVRCP_EVENT_NOTIFICATION_IND: {
            bt_avrcp_event_notification_t *noti_ind = (bt_avrcp_event_notification_t *)buffer;
            ret = bt_sink_srv_avrcp_handle_event_notification_ind(noti_ind, status);
            break;
        }

        case BT_AVRCP_REGISTER_NOTIFICATION_IND:
        {
            bt_avrcp_register_notification_commant_t *register_noti_ind = (bt_avrcp_register_notification_commant_t *)buffer;
            ret = bt_sink_srv_avrcp_handle_register_notification_ind(register_noti_ind, status);
            break;
        }

        case BT_AVRCP_SET_ABSOLUTE_VOLUME_COMMAND_IND:
        {
            bt_avrcp_set_absolute_volume_event_t *set_vol_ind = (bt_avrcp_set_absolute_volume_event_t *)buffer;
            ret = bt_sink_srv_avrcp_handle_set_absolute_volume_ind(set_vol_ind, status);
            break;
        }

        case BT_AVRCP_GET_CAPABILITY_IND: {
            uint32_t conn_handle = (uint32_t)buffer;
            bt_avrcp_capability_attributes_response_t rsp = {0};

            bt_avrcp_event_t capability_attr_list[1] = {
                BT_AVRCP_EVENT_VOLUME_CHANGED
            };

            rsp.handle = conn_handle;
            rsp.length = 2;
            rsp.number = 1;
            rsp.attribute_list = capability_attr_list;
            bt_avrcp_send_capability_response(conn_handle, &rsp, BT_AVRCP_CAPABILITY_EVENTS_SUPPORTED);
            break;
        }

        default:
            break;
    }

    bt_sink_srv_report("[sink][avrcp]common_hdr[e]-ret: %d", ret);

    return ret;
}


int32_t bt_sink_srv_avrcp_play_music(uint32_t handle)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
    bt_sink_srv_music_device_t *dev = NULL;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AVRCP_HD, (void *)(&handle));
    if ((dev) && (dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
        ret = bt_avrcp_send_pass_through_command(handle, BT_AVRCP_OPERATION_ID_PLAY, 0);
    }
    bt_sink_srv_report("[sink][avrcp]play_music-hd: 0x%x, ret: %d", handle, ret);
    return ret;
}


int32_t bt_sink_srv_avrcp_stop_music(uint32_t handle)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
    bt_sink_srv_music_device_t *dev = NULL;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AVRCP_HD, (void *)(&handle));
    if ((dev) && (dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
        ret = bt_avrcp_send_pass_through_command(handle, BT_AVRCP_OPERATION_ID_PAUSE, 0);
    }
    bt_sink_srv_report("[sink][avrcp]stop_music-hd: 0x%x, ret: %d", handle, ret);
    return ret;
}


int32_t bt_sink_srv_avrcp_change_ntrack(uint32_t handle)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
    bt_sink_srv_music_device_t *dev = NULL;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AVRCP_HD, (void *)(&handle));
    if ((dev) && (dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
        ret = bt_avrcp_send_pass_through_command(handle, BT_AVRCP_OPERATION_ID_FORWARD, 0);
    }
    bt_sink_srv_report("[sink][avrcp]change_ntrack-hd: 0x%x, ret: %d", handle, ret);
    return ret;
}


int32_t bt_sink_srv_avrcp_change_ptrack(uint32_t handle)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
    bt_sink_srv_music_device_t *dev = NULL;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AVRCP_HD, (void *)(&handle));
    if ((dev) && (dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
        ret = bt_avrcp_send_pass_through_command(handle, BT_AVRCP_OPERATION_ID_BACKWARD, 0);
    }
    bt_sink_srv_report("[sink][avrcp]change_ptrack-hd: 0x%x, ret: %d", handle, ret);
    return ret;
}

