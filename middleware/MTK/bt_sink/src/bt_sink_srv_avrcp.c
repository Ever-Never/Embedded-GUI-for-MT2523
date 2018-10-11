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

SinkProfileAvrcp sink_avrcp[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];

#ifndef MTK_DEBUG_LEVEL_INFO
extern int32_t g_test_fix_warning;
#endif
extern bool bt_sink_srv_a2dp_check_dev_flag(bt_sink_srv_music_device_t *dev, uint32_t flag);
extern void bt_sink_srv_set_music_pause_state(int32_t state);
extern uint32_t bt_sink_srv_a2dp_stop_music(uint32_t user_handle, uint32_t duration_ms);
extern int32_t bt_sink_srv_get_music_pause_state(void);
extern uint32_t bt_sink_srv_open_a2dp_codec(void);
static void bt_sink_srv_avrcp_init()
{
    uint32_t i;

    for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; i++) {
        sink_avrcp[i].profile.uuid = BT_SINK_SRV_UUID_AVRCP;
        sink_avrcp[i].profile.state = BT_SINK_SRV_AVRCP_DISCONNECTED;
    }
}


static void bt_sink_srv_avrcp_op_timer_hdl(TimerHandle_t timer_hd)
{
    bt_status_t ret = -111;
    bt_sink_srv_music_device_t *dev = NULL;
    uint32_t avrcp_op = 0, dev_flag = 0xffff;
    bt_sink_srv_music_context_t *cntx = NULL;
    uint32_t dev_a2dp_hd = 0;

    cntx = bt_sink_srv_music_get_context();
    //dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LATEST, NULL);
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);

    avrcp_op = cntx->avrcp_op;

    if ((dev) && (dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
        dev_flag = dev->flag;
        dev_a2dp_hd = dev->a2dp_hd;

        if (cntx->avrcp_op & BT_SINK_SRV_MUSIC_AVRCP_OP_PLAY) {
            cntx->avrcp_op &= ~(BT_SINK_SRV_MUSIC_AVRCP_OP_PLAY);
            ret = bt_sink_srv_avrcp_play_music(dev->avrcp_hd);
            /* update dev flag */
            dev->flag |= BT_SINK_SRV_MUSIC_FLAG_AVRCP_PLAY;
            dev->flag &= (~BT_SINK_SRV_MUSIC_FLAG_AVRCP_PAUSE);
            /* set streaming state */
            bt_sink_srv_state_set(BT_SINK_SRV_STATE_STREAMING);
        } else if (cntx->avrcp_op & BT_SINK_SRV_MUSIC_AVRCP_OP_PAUSE) {
            cntx->avrcp_op &= ~(BT_SINK_SRV_MUSIC_AVRCP_OP_PAUSE);
            ret = bt_sink_srv_avrcp_stop_music(dev->avrcp_hd);
            /* update dev flag */
            dev->flag &= (~BT_SINK_SRV_MUSIC_FLAG_AVRCP_PLAY);
            dev->flag |= BT_SINK_SRV_MUSIC_FLAG_AVRCP_PAUSE;
            /* set streaming state */
            bt_sink_srv_state_reset(BT_SINK_SRV_STATE_STREAMING);
        } else {
            ret = -112;
        }
    } else {
        ret = -114;
    }

    if (cntx->timer) {
        xTimerStop(cntx->timer, 0);
        xTimerDelete(cntx->timer, 0);
        cntx->timer = NULL;
    }

    bt_sink_srv_report("[sink][avrcp]op_timer_hdl-op: 0x%x, flag: 0x%x, dev_a2dp_hd: 0x%x, ctx_a2dp_hd: 0x%x, state: %d, ret: %d\n",
                       avrcp_op, dev_flag, dev_a2dp_hd, cntx->a2dp_hd, cntx->state, ret);

    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = (int32_t) dev_a2dp_hd;
    g_test_fix_warning = (int32_t)dev_flag;
    g_test_fix_warning = (int32_t)avrcp_op;
    g_test_fix_warning = ret;
    #endif
}


bt_sink_srv_status_t bt_sink_srv_avrcp_action_handler(bt_sink_srv_action_t action, void *param)
{
    bt_status_t ret = -101;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;
    bt_sink_srv_profile_connection_action_t *conn_info = NULL;
    bt_bd_addr_t *dev_addr = NULL;
    int32_t err = 0;

    bt_sink_srv_report("[sink][avrcp]process_avrcp_action[s]-event: 0x%x, base: 0x%x\n", action, BT_SINK_SRV_ACTION_AVRCP_START);

    cntx = bt_sink_srv_music_get_context();

    switch (action) {
        case BT_SINK_SRV_ACTION_PROFILE_INIT: {
            bt_sink_srv_avrcp_init();
            break;
        }

        case BT_SINK_SRV_ACTION_PROFILE_DISCONNECT: {
            conn_info = (bt_sink_srv_profile_connection_action_t *)param;
            dev_addr = &(conn_info->bt_addr);
            if (conn_info->profile_connection_mask & BT_SINK_SRV_PROFILE_CONNECTION_TYPE_AVRCP) {
                dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR, dev_addr);
                if ((dev) && (dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
                    ret = bt_avrcp_disconnect(dev->avrcp_hd);
                } else {
                    err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
                }
            }
            break;
        }

        case BT_SINK_SRV_ACTION_PLAY: {
            #if 0
            if (!cntx->timer) {
                cntx->timer = xTimerCreate("AVRCP_OP_Timer", 0xffff, pdFALSE, (void *)0, bt_sink_srv_avrcp_op_timer_hdl);
                xTimerChangePeriod(cntx->timer, BT_SINK_SRV_AVRCP_OP_TIMER_DUR / portTICK_PERIOD_MS, 0);
                xTimerReset(cntx->timer, 0);
            } else {
                xTimerChangePeriod(cntx->timer, BT_SINK_SRV_AVRCP_OP_TIMER_DUR / portTICK_PERIOD_MS, 0);
                xTimerReset(cntx->timer, 0);
            }
            #endif

            if (cntx->avrcp_op & BT_SINK_SRV_MUSIC_AVRCP_OP_PLAY) {
                ret = -180;
            } else {
                cntx->avrcp_op |= BT_SINK_SRV_MUSIC_AVRCP_OP_PLAY;
                ret = -181;
            }

            bt_sink_srv_avrcp_op_timer_hdl(NULL);

            break;
        }

        case BT_SINK_SRV_ACTION_PAUSE: {
            #if 0
            if (!cntx->timer) {
                cntx->timer = xTimerCreate("AVRCP_OP_Timer", 0xffff, pdFALSE, (void *)0, bt_sink_srv_avrcp_op_timer_hdl);
                xTimerChangePeriod(cntx->timer, BT_SINK_SRV_AVRCP_OP_TIMER_DUR / portTICK_PERIOD_MS, 0);
                xTimerReset(cntx->timer, 0);
            } else {
                xTimerChangePeriod(cntx->timer, BT_SINK_SRV_AVRCP_OP_TIMER_DUR / portTICK_PERIOD_MS, 0);
                xTimerReset(cntx->timer, 0);
            }
            #endif

            if (cntx->avrcp_op & BT_SINK_SRV_MUSIC_AVRCP_OP_PAUSE) {
                ret = -190;
            } else {
                cntx->avrcp_op |= BT_SINK_SRV_MUSIC_AVRCP_OP_PAUSE;
                ret = -191;
            }

            bt_sink_srv_avrcp_op_timer_hdl(NULL);

            break;
        }

        case BT_SINK_SRV_ACTION_NEXT_TRACK: {
            //dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_USED, NULL);
            //dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LATEST, NULL);
            dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);
            if (dev) {
                ret = bt_sink_srv_avrcp_change_ntrack(dev->avrcp_hd);
            }
            break;
        }

        case BT_SINK_SRV_ACTION_PRE_TRACK: {
            //dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_USED, NULL);
            //dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LATEST, NULL);
            dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);
            if (dev) {
                ret = bt_sink_srv_avrcp_change_ptrack(dev->avrcp_hd);
            }
            break;
        }

        default:
            break;
    }

    bt_sink_srv_report("[sink][avrcp]process_avrcp_action[e]-ret: %d, err: %d\n", ret, err);
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = ret;
    g_test_fix_warning = err;
    #endif
    return BT_SINK_SRV_STATUS_SUCCESS;
}


bt_status_t bt_sink_srv_avrcp_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    int32_t ret = 0;
    SinkProfileAvrcp *avrcp = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    //bt_sink_srv_avrcp_event_t avrcp_event;
    bt_sink_srv_avrcp_disconnect_t avrcp_disconnect;
    bt_sink_srv_avrcp_connect_t avrcp_connect;
    bt_bd_addr_t dev_addr;
    bool connected = false;

    bt_sink_srv_report("[sink][avrcp]common_hdr[s]-msg: 0x%x, status: %d\n", msg, status);

    switch (msg) {
        case BT_AVRCP_CONNECT_CNF: {
            bt_avrcp_connect_cnf_t *conn_cnf = (bt_avrcp_connect_cnf_t *)buffer;

            dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AVRCP_HD, (void *)(&(conn_cnf->handle)));
            memcpy(&dev_addr, &(dev->dev_addr), sizeof(bt_bd_addr_t));

            bt_sink_srv_report("[sink][avrcp]connect_cnf-avrcp_hd: 0x%x\n", conn_cnf->handle);

            if (status == BT_STATUS_SUCCESS) {
                /* avrcp connected */
                dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AVRCP_HD, (void *)(&(conn_cnf->handle)));
                if (dev) {
                    ;
                } else {
                    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_UNUSED, NULL);
                    dev->avrcp_hd = conn_cnf->handle;
                }
                dev->conn_bit |= BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT;
                /* fill avrcp notify event */
                avrcp_connect.ret = 0;
                avrcp_connect.handle = dev->avrcp_hd;
                memcpy(&(avrcp_connect.dev_addr), &(dev->dev_addr), sizeof(bt_bd_addr_t));
                avrcp = (SinkProfileAvrcp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AVRCP);
                if (avrcp) {
                    avrcp->profile.state = BT_SINK_SRV_AVRCP_CONNECTED;
                } else {
                    // trace error
                }

                connected = true;
        #if defined(__BT_AWS_SUPPORT__) && defined(__MUSIC_MODE_SWITCH_SUPPORT__)
                bt_avrcp_register_notification(conn_cnf->handle, BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED, 20);  
        #endif
            } else {
                avrcp_connect.ret = -1;
            }

            avrcp_connect.ind = true;
            /* notify a2dp event */
            //avrcp_event.event = BT_SINK_SRV_AVRCP_EVENT_CONNECT;
            //avrcp_event.param = &avrcp_connect;
            bt_sink_srv_cm_profile_status_notify(&dev_addr, BT_SINK_SRV_TYPE_AVRCP, connected);
       #ifdef MTK_BT_COMMAND_ENABLE
            if (avrcp_connect.ret == 0) {
                bt_sink_srv_cm_profile_state_change_notify(&(dev->dev_addr), 
                        BT_SINK_SRV_CM_PROFILE_AVRCP, BT_SINK_SRV_CM_STATE_CONNECTED);
            } else {
                bt_sink_srv_cm_profile_state_change_notify(&(dev->dev_addr), 
                        BT_SINK_SRV_CM_PROFILE_AVRCP, BT_SINK_SRV_CM_STATE_DISCONNECTED);
            }
        #endif 
            break;
        }

        case BT_AVRCP_CONNECT_IND: {
            bt_avrcp_connect_ind_t *conn_ind = (bt_avrcp_connect_ind_t *)buffer;

            bt_sink_srv_report("[sink][avrcp]connect_ind-avrcp_hd: 0x%x\n", conn_ind->handle);

            dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR, conn_ind->address);
            if (dev) {
                ;
            } else {
                dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_UNUSED, NULL);
                bt_sink_srv_memcpy(&(dev->dev_addr), conn_ind->address, sizeof(bt_bd_addr_t));
            }
            dev->avrcp_hd = conn_ind->handle;
            ret = bt_avrcp_connect_response(conn_ind->handle, true);
       #ifdef MTK_BT_COMMAND_ENABLE    
            bt_sink_srv_cm_profile_state_change_notify(&(dev->dev_addr), 
                    BT_SINK_SRV_CM_PROFILE_AVRCP, BT_SINK_SRV_CM_STATE_CONNECTING);
        #endif
            break;
        }

        case BT_AVRCP_DISCONNECT_IND: {
            bt_avrcp_disconnect_ind_t *disconn_ind = (bt_avrcp_disconnect_ind_t *)buffer;

            dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AVRCP_HD, (void *)(&(disconn_ind->handle)));
            memcpy(&dev_addr, &(dev->dev_addr), sizeof(bt_bd_addr_t));

            if (dev) {
                dev->conn_bit &= ~(BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT);
                /* fill avrcp notify event */
                avrcp_disconnect.ret = 0;
                avrcp_disconnect.handle = dev->avrcp_hd;
                memcpy(&(avrcp_disconnect.dev_addr), &(dev->dev_addr), sizeof(bt_bd_addr_t));
                if (!(dev->conn_bit)) {
                    bt_sink_srv_music_reset_device(dev);
                }
            }
            avrcp = (SinkProfileAvrcp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AVRCP);
            if (avrcp) {
                avrcp->profile.state = BT_SINK_SRV_AVRCP_DISCONNECTED;
            } else {
                // trace error
            }

            avrcp_disconnect.ind = true;
            /* notify a2dp event */
            //avrcp_event.event = BT_SINK_SRV_AVRCP_EVENT_DISCONNECT;
            //avrcp_event.param = &avrcp_disconnect;
            bt_sink_srv_cm_profile_status_notify(&dev_addr, BT_SINK_SRV_TYPE_AVRCP, false);
        #ifdef MTK_BT_COMMAND_ENABLE
            bt_sink_srv_cm_profile_state_change_notify(&(dev->dev_addr), 
                    BT_SINK_SRV_CM_PROFILE_AVRCP, BT_SINK_SRV_CM_STATE_DISCONNECTED);
        #endif
            break;
        }

        case BT_AVRCP_PASS_THROUGH_CNF: {
            bt_avrcp_pass_through_cnf_t *through_cnf = (bt_avrcp_pass_through_cnf_t *)buffer;

            if (status == BT_STATUS_SUCCESS) {
                if (through_cnf->op_state == 0) {
                    ret = bt_avrcp_send_pass_through_command(through_cnf->handle, through_cnf->op_id, 1);
                } else if (through_cnf->op_state == 1) {
                    /* ignore */
                }
            }
            break;
        }
        
#if defined(__BT_AWS_SUPPORT__) && defined(__MUSIC_MODE_SWITCH_SUPPORT__)
        case BT_AVRCP_EVENT_NOTIFICATION_IND: {
            bt_avrcp_event_notification_t *not_ind = (bt_avrcp_event_notification_t *)buffer;

            if (BT_STATUS_SUCCESS != status && BT_STATUS_AVRCP_INTERIM != status) {
                bt_sink_srv_report("[sink][avrcp] avrcp notification fail, evt_id:%d, reason:0x%08X",
                                                not_ind->event_id, status);            
            } else {
                if (BT_STATUS_AVRCP_INTERIM == status) {
                    bt_sink_srv_report("[sink][avrcp] avrcp notification interim, evt_id:%d",not_ind->event_id);
                } else {
                    if (not_ind && not_ind->event_id == BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED) {
                        bt_sink_srv_music_context_t *cntx = bt_sink_srv_music_get_context();
                        bt_sink_srv_music_device_t *dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&cntx->a2dp_hd));
                        bool codec_opened_flag = bt_sink_srv_a2dp_check_dev_flag(dev, BT_SINK_SRV_MUSIC_FLAG_AMI_OPEN_CODEC);
                        bool data_discard_flag = bt_sink_srv_a2dp_check_dev_flag(dev, BT_SINK_SRV_MUSIC_FALG_A2DP_DISCARD_DATA);

                        bt_sink_srv_report("[sink][avrcp] avrcp notification changed, evt_id:%d, now_status:%d", not_ind->event_id, not_ind->status);
                        // do noting if it's mode changing
                        if (cntx->flag & BT_SINK_SRV_MUSIC_FLAG_AWS_MODE_CHANGING) {
                            ret = bt_avrcp_register_notification(not_ind->handle, BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED, 20);
                            bt_sink_srv_report("[sink][avrcp] mode changing, do nothing, ret = %d", ret);
                            break;
                        }
                        if (bt_sink_srv_ami_get_music_mode() == BT_SINK_SRV_MODE_BT_MUSIC){
                            if (not_ind->status == 0x01) {
                                if (false == codec_opened_flag && true == data_discard_flag) {
                                    bt_sink_srv_music_device_t *aws_dev = NULL;
                                    SinkProfileA2dp *a2dp = NULL;
                                    aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, NULL);
                                    a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
                                    bt_aws_codec_capability_t aws_cap;
                                    if ((aws_dev) &&
                                        (aws_dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT) &&
                                        (aws_dev->aws_role == BT_AWS_ROLE_SOURCE)) {
                                        /* AWS connection, translate start streaming ind */
                                        if (a2dp->codec.type == BT_A2DP_CODEC_SBC) {
                                            aws_cap.type = BT_AWS_CODEC_TYPE_SBC;
                                        } else if (a2dp->codec.type == BT_A2DP_CODEC_AAC) {
                                            aws_cap.type = BT_AWS_CODEC_TYPE_AAC;
                                        } else {
                                            /* trace error */
                                        }

                                        aws_cap.length = a2dp->codec.length;
                                        memcpy(&(aws_cap.codec), &(a2dp->codec.codec), a2dp->codec.length);
                                        ret = bt_aws_start_streaming_request(aws_dev->aws_hd, &aws_cap);
                                        bt_sink_srv_report("[sink][avrcp] aws start streaming, ret:%d", ret);
                                    } else if ((aws_dev) &&
                                        (aws_dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT) &&
                                        (aws_dev->aws_role != BT_AWS_ROLE_SOURCE)) {
                                        cntx->flag |= BT_SINK_SRV_MUSIC_FLAG_AWS_WAIT_ROLE_STREAMING;
                                        ret = bt_aws_set_role_request(aws_dev->aws_hd, BT_AWS_ROLE_SOURCE);
                                        bt_sink_srv_report("[sink][avrcp] aws set role, ret:%d", ret);
                                    }
                                    bt_sink_srv_open_a2dp_codec();
                                }
                            }
                        } else if (bt_sink_srv_ami_get_music_mode() == BT_SINK_SRV_MODE_LOCAL_MUSIC) {
                            if (not_ind->status == 0x01) {
                                int32_t music_state = bt_sink_srv_get_music_pause_state();
                                if (SINK_MUSIC_STATE_PAUSE_AVRCP == music_state) {
                                    bt_sink_srv_a2dp_stop_music(not_ind->handle, 80);
                                }
                            }
                        }
                        ret = bt_avrcp_register_notification(not_ind->handle, BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED, 20);
                    }
                }
            }
            break;
        }
    #endif
        default:
            break;
    }

    bt_sink_srv_report("[sink][avrcp]common_hdr[e]-ret: %d\n", ret);

    return ret;
}


int32_t bt_sink_srv_avrcp_play_music(uint32_t handle)
{
    int32_t ret = -3;
    bt_sink_srv_music_device_t *dev = NULL;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AVRCP_HD, (void *)(&handle));
    if ((dev) && (dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
        ret = bt_avrcp_send_pass_through_command(handle, BT_AVRCP_OPERATION_ID_PLAY, 0);
    }
    bt_sink_srv_report("[sink][avrcp]play_music-hd: 0x%x, ret: %d\n", handle, ret);
    return ret;
}


int32_t bt_sink_srv_avrcp_stop_music(uint32_t handle)
{
    int32_t ret = -3;
    bt_sink_srv_music_device_t *dev = NULL;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AVRCP_HD, (void *)(&handle));
    if ((dev) && (dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
        ret = bt_avrcp_send_pass_through_command(handle, BT_AVRCP_OPERATION_ID_PAUSE, 0);
    }
    bt_sink_srv_set_music_pause_state(SINK_MUSIC_STATE_PAUSE_AVRCP);
    bt_sink_srv_report("[sink][avrcp]stop_music-hd: 0x%x, ret: %d\n", handle, ret);
    return ret;
}


int32_t bt_sink_srv_avrcp_change_ntrack(uint32_t handle)
{
    int32_t ret = -3;
    bt_sink_srv_music_device_t *dev = NULL;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AVRCP_HD, (void *)(&handle));
    if ((dev) && (dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
        ret = bt_avrcp_send_pass_through_command(handle, BT_AVRCP_OPERATION_ID_FORWARD, 0);
    }
    bt_sink_srv_report("[sink][avrcp]change_ntrack-hd: 0x%x, ret: %d\n", handle, ret);
    return ret;
}


int32_t bt_sink_srv_avrcp_change_ptrack(uint32_t handle)
{
    int32_t ret = -3;
    bt_sink_srv_music_device_t *dev = NULL;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AVRCP_HD, (void *)(&handle));
    if ((dev) && (dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
        ret = bt_avrcp_send_pass_through_command(handle, BT_AVRCP_OPERATION_ID_BACKWARD, 0);
    }
    bt_sink_srv_report("[sink][avrcp]change_ptrack-hd: 0x%x, ret: %d\n", handle, ret);
    return ret;
}

