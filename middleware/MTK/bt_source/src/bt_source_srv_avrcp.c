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

#include "bt_avrcp.h"
#include "bt_source_srv.h"
#include "bt_source_srv_avrcp.h"
#include "bt_source_srv_utils.h"
#include "bt_source_srv_cntx.h"

#define BT_SOURCE_AVRCP_CAPABILITY_EVENT_SUPPORTED_LENGTH 7 // we support 7 total capability of the application 

bt_status_t bt_source_srv_avrcp_connect(bt_bd_addr_t *device_addr)
{
    int32_t err = 0;
    uint32_t hd = 0;
    uint32_t role = BT_AVRCP_ROLE_TG;
    bt_status_t ret = BT_STATUS_FAIL;
    bt_source_srv_device_t *dev = NULL;

    dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_ADDR, (const void *)device_addr);
    if (dev) { /** dev have existed. */
        if ((dev->conn_bit & BT_SOURCE_SRV_AVRCP_CONN_BIT) && (BT_SOURCE_SRV_INVALID_HD != dev->avrcp_hd)) {
            err = -99;
            bt_source_srv_report("[source][avrcp]connect: ret: 0x%x, errcode: %d\r\n", ret, err);
            return ret;
        }
        err = -100;
    } else {
        dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_UNUSED, NULL);
    }
    if (dev) {  
        bt_avrcp_init_struct bt_role;
        bt_role.role = role;
        bt_avrcp_init(&bt_role);
        ret = bt_avrcp_connect(&hd, (const bt_bd_addr_t *)device_addr);
        if (BT_STATUS_SUCCESS == ret) {/** normal case. */
            dev->avrcp_hd = hd;
            dev->avrcp_role = role;
            bt_source_srv_memcpy(&(dev->dev_addr), device_addr, sizeof(bt_bd_addr_t));
            err = 0;
        }
    } else {
        /** unexpected, no unused device, please check log. */
        err = -101;
    }

    bt_source_srv_report("[source][avrcp]connect: ret: 0x%x, errcode: %d\r\n", ret, err);
    return ret;
}

bt_status_t bt_source_srv_avrcp_disconnect(uint32_t handle)
{
    int32_t err = 0;
    bt_status_t ret = BT_STATUS_FAIL;
    bt_source_srv_device_t *dev = NULL;

    dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_AVRCP, (const void *)(&handle));
    if ((dev) && (dev->conn_bit & BT_SOURCE_SRV_AVRCP_CONN_BIT)) {
        ret = bt_avrcp_disconnect(dev->avrcp_hd);
    } else { /** un-normal case. */
        err = -102;
    }
    
    bt_source_srv_report("[source][avrcp]disconnect-handle: 0x%x, ret: 0x%x, errcode: %d\r\n", handle, ret, err);
    return ret;
}

bt_status_t bt_source_srv_avrcp_change_volume(uint32_t handle, uint8_t volume)
{
    bt_status_t ret = BT_STATUS_FAIL;
    bt_source_srv_device_t *dev = NULL;
    uint8_t volumePercentage = (volume*0x7F)/100;

    dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_AVRCP, (const void *)(&handle));
    if ((dev) && (dev->conn_bit & BT_SOURCE_SRV_AVRCP_CONN_BIT)) {
        ret = bt_avrcp_set_absolute_volume(handle, volumePercentage);
    }
    bt_source_srv_report("[source][avrcp]change-voulme-hd: 0x%x, ret: 0x%x, volume: %d\r\n", handle, ret, volume);
    return ret;
}

bt_status_t bt_source_srv_avrcp_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    bt_source_srv_device_t *dev = NULL;
    bt_status_t sta = BT_STATUS_SUCCESS;
    bt_bd_addr_t address = {0};

    bt_source_srv_report("[source][avrcp]common_hdr[s]-msg: 0x%x, status:0x%x\r\n", msg, status);

    switch (msg) {
        case BT_AVRCP_CONNECT_CNF: {
            bt_avrcp_connect_cnf_t *conn_cnf = (bt_avrcp_connect_cnf_t *)buffer;
            bt_source_srv_report("[source][avrcp]connect_cnf-avrcp_hd: 0x%x\r\n", conn_cnf->handle);

            if (status == BT_STATUS_SUCCESS) {/** avrcp connected. */
                dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_AVRCP, (const void *)(&(conn_cnf->handle)));

                if (dev) {
                    dev->avrcp_state = BT_SOURCE_SRV_AVRCP_CONNECTED;
                    dev->conn_bit |= BT_SOURCE_SRV_AVRCP_CONN_BIT;
                    bt_source_srv_memcpy((void *)(&(address)),(const void *)(&(dev->dev_addr)), sizeof(bt_bd_addr_t));
                }
                /** TODO when to invoke firstly. */
                sta = bt_avrcp_register_notification(conn_cnf->handle, BT_AVRCP_EVENT_VOLUME_CHANGED, 0);
                bt_source_srv_report("[source][avrcp]connect_cnf-register_noti sta: 0x%x\n", sta);
            } else {/** connect fail, so reset the cntx. */
                dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_AVRCP, (const void *)(&(conn_cnf->handle)));
                if (dev) {   
                    dev->conn_bit &= ~(BT_SOURCE_SRV_AVRCP_CONN_BIT);
                    bt_source_srv_reset_avrcp_info(dev);
                    
                    if (!(dev->conn_bit)) {
                        bt_source_srv_reset_device(dev);
                    }
                }
            }
            /** To notify app. */
            {
                bt_source_srv_avrcp_connect_t param;
                param.handle = conn_cnf->handle;
                param.result = status;
                bt_source_srv_memcpy((void *)(&(param.address)),(const void *)address, sizeof(bt_bd_addr_t));
                bt_source_srv_event_callback(BT_SOURCE_SRV_EVENT_AVRCP_CONNECT, (void *)(&param));
            }
            break;
        }

        case BT_AVRCP_CONNECT_IND: {
            bt_avrcp_connect_ind_t *conn_ind = (bt_avrcp_connect_ind_t *)buffer;

            if (BT_STATUS_SUCCESS == status) {
                dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_ADDR, (const void *)conn_ind->address);
                if (dev) {
                    ;
                } else {
                    dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_UNUSED, NULL);
                }
                if (dev) {
                    dev->avrcp_role = BT_AVRCP_ROLE_TG;
                    dev->avrcp_hd = conn_ind->handle;
                    bt_source_srv_memcpy(&(dev->dev_addr), conn_ind->address, sizeof(bt_bd_addr_t));
                    
                    ret = bt_avrcp_connect_response(conn_ind->handle, true);
                } else {
                    ret = bt_avrcp_connect_response(conn_ind->handle, false);
                }
            }
            
            bt_source_srv_report("[source][avrcp]connect_ind-avrcp_hd: 0x%x, ret 0x%x\r\n", conn_ind->handle, ret);
            break;
        }

        case BT_AVRCP_DISCONNECT_IND: {
            bt_bd_addr_t address = {0};
            bt_avrcp_disconnect_ind_t *disconn_ind = (bt_avrcp_disconnect_ind_t *)buffer;
            bt_source_srv_report("[source][avrcp]disconnect_ind-avrcp_hd: 0x%x\n", disconn_ind->handle);
            
            dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_AVRCP, (const void *)(&(disconn_ind->handle)));
            if (dev) {
                bt_source_srv_memcpy((void *)(&(address)),(const void *)(&(dev->dev_addr)), sizeof(bt_bd_addr_t));                
                dev->conn_bit &= ~(BT_SOURCE_SRV_AVRCP_CONN_BIT);
                bt_source_srv_reset_avrcp_info(dev);
                if (!(dev->conn_bit)) {
                    bt_source_srv_reset_device(dev);
                }
            }
            /** To notify app. */
            {
                bt_source_srv_avrcp_disconnect_t param;
                param.handle = disconn_ind->handle;
                param.result= status;
                bt_source_srv_memcpy((void *)(&(param.address)),(const void *)address, sizeof(bt_bd_addr_t));
                bt_source_srv_event_callback(BT_SOURCE_SRV_EVENT_AVRCP_DISCONNECT, (void *)(&param));
            }
            break;
        }

        case BT_AVRCP_PASS_THROUGH_COMMAND_IND: {
            bt_avrcp_pass_through_command_ind_t *through_ind = (bt_avrcp_pass_through_command_ind_t *)buffer;

            dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_AVRCP, (const void *)(&(through_ind->handle)));
            if ((status == BT_STATUS_SUCCESS) && dev &&
                (dev->conn_bit & BT_SOURCE_SRV_A2DP_CONN_BIT)) {
                if ((through_ind->op_id == BT_AVRCP_OPERATION_ID_PLAY) ||
                    (through_ind->op_id == BT_AVRCP_OPERATION_ID_STOP) ||
                    (through_ind->op_id == BT_AVRCP_OPERATION_ID_PAUSE) ||
                    (through_ind->op_id == BT_AVRCP_OPERATION_ID_FORWARD) ||
                    (through_ind->op_id == BT_AVRCP_OPERATION_ID_BACKWARD) ||
                    (through_ind->op_id == BT_AVRCP_OPERATION_ID_VOLUME_UP) ||
                    (through_ind->op_id == BT_AVRCP_OPERATION_ID_VOLUME_DOWN)) {
                    bt_avrcp_send_pass_through_response(through_ind->handle, BT_AVRCP_RESPONSE_ACCEPTED, through_ind->op_id, through_ind->op_state);
                    
                    if (through_ind->op_state == BT_AVRCP_OPERATION_STATE_PUSH) {/** To notify app. */
                        bt_source_srv_avrcp_operation_ind_t avrcp_op;
                        avrcp_op.handle = through_ind->handle;
                        avrcp_op.op_id = through_ind->op_id;
                        bt_source_srv_event_callback(BT_SOURCE_SRV_EVENT_AVRCP_OPERATION_IND, (void *)(&avrcp_op));
                    }  else if (through_ind->op_state == BT_AVRCP_OPERATION_STATE_RELEASED) {
                        /** To ignore. */
                    }
                } 
            } else {  
                bt_avrcp_send_pass_through_response(through_ind->handle, BT_AVRCP_RESPONSE_REJECTED, through_ind->op_id, through_ind->op_state);
            }
            break;
        }
        
        case BT_AVRCP_SET_ABSOLUTE_VOLUME_CNF:{
            bt_avrcp_set_absolute_volume_response_t *vol_cnf = (bt_avrcp_set_absolute_volume_response_t *)buffer;

            dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_AVRCP, (const void *)(&(vol_cnf->handle)));
            if ((BT_STATUS_SUCCESS == status) && dev) {/** To notify app. */
                bt_source_srv_avrcp_change_volume_cnf_t param;
                uint8_t volumePercentage = vol_cnf->volume;
                
                volumePercentage = (volumePercentage*100)/0x7F;
                param.handle = vol_cnf->handle;
                param.volume = volumePercentage;
                bt_source_srv_event_callback(BT_SOURCE_SRV_EVENT_AVRCP_CHANGE_VOLUME_CNF, (void *)(&param));
            }
            break;
        }
        
        case BT_AVRCP_EVENT_NOTIFICATION_IND:{  
            uint8_t volumePercentage = 0;
            bt_source_srv_avrcp_remote_volume_ind_t param = {0};
            bt_avrcp_event_notification_t *noti_ind = (bt_avrcp_event_notification_t *)buffer;

            dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_AVRCP, (const void *)(&(noti_ind->handle)));
            if ((BT_AVRCP_EVENT_VOLUME_CHANGED == noti_ind->event_id) && dev) {
                if (BT_STATUS_AVRCP_INTERIM == status) {/** sync volume, Change the volume representation by percentage, and To notify application. */
                    volumePercentage = noti_ind->volume;
                    volumePercentage = (volumePercentage*100)/0x7F;
                    param.handle = noti_ind->handle;
                    param.volume = volumePercentage;
                    bt_source_srv_event_callback(BT_SOURCE_SRV_EVENT_AVRCP_REMOTE_VOLUME_IND, (void *)(&param));
                } else if (BT_STATUS_SUCCESS == status) {
                    /** The registered notification is changed.
                     * An additional NOTIFY command is expected to be sent.
                     * Handle the notification.
                     */
                    volumePercentage = noti_ind->volume;
                    volumePercentage = (volumePercentage*100)/0x7F;
                    param.handle = noti_ind->handle;
                    param.volume = volumePercentage;
                    bt_source_srv_event_callback(BT_SOURCE_SRV_EVENT_AVRCP_REMOTE_VOLUME_IND, (void *)(&param));
                    sta = bt_avrcp_register_notification(noti_ind->handle, BT_AVRCP_EVENT_VOLUME_CHANGED, 0);
                    bt_source_srv_report("[source][avrcp]pass cmd-register_noti sta: 0x%x\r\n", sta);
                    
                } else {/** error handling. */
                    sta = bt_avrcp_register_notification(noti_ind->handle, BT_AVRCP_EVENT_VOLUME_CHANGED, 0);
                    bt_source_srv_report("[source][avrcp]pass cmd-register_noti again sta: 0x%x\r\n", sta);
                }
            }
            break;
        }

        case BT_AVRCP_GET_PLAY_STATUS_NOTIFICATION_IND: {
            bt_avrcp_get_play_status_commant_t *noti_ind = (bt_avrcp_get_play_status_commant_t *)buffer;
            bt_avrcp_media_play_status_notification_t rsp = {0};

            dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_AVRCP, (const void *)(&(noti_ind->handle)));
            if (dev) {
                rsp.bt_avrcp_media_play_status_event_t = dev->avrcp_play_status; //current media state
                if (BT_AVRCP_EVENT_MEDIA_PLAY_STOPPED != rsp.bt_avrcp_media_play_status_event_t) {
                    rsp.song_length = 2;
                    rsp.song_position = 1;
                }
            }

            bt_avrcp_media_send_play_status_response(noti_ind->handle, &rsp);
            break;
        }

        case BT_AVRCP_GET_CAPABILITY_IND: {
            uint32_t conn_handle = (uint32_t)buffer;
            bt_avrcp_capability_attributes_response_t rsp = {0};
            
            bt_avrcp_event_t capability_attr_list[BT_SOURCE_AVRCP_CAPABILITY_EVENT_SUPPORTED_LENGTH] = {
                BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED,
                BT_AVRCP_EVENT_TRACK_CHANGED,
                BT_AVRCP_EVENT_PLAYBACK_POS_CHANGED,
                BT_AVRCP_EVENT_PLAYER_APP_SETTING_CHANGED,
                BT_AVRCP_EVENT_NOW_PLAYING_CONTENT_CHANGED,
                BT_AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED,
                BT_AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED
            };
            
            rsp.handle = conn_handle;
            rsp.length = BT_SOURCE_AVRCP_CAPABILITY_EVENT_SUPPORTED_LENGTH + 2;
            rsp.number = BT_SOURCE_AVRCP_CAPABILITY_EVENT_SUPPORTED_LENGTH;
            rsp.attribute_list = capability_attr_list;
            bt_avrcp_send_capability_response(conn_handle, &rsp, BT_AVRCP_CAPABILITY_EVENTS_SUPPORTED);
            break;
        }
        
    	case BT_AVRCP_REGISTER_NOTIFICATION_IND:
    	{ 
            bt_avrcp_send_register_notification_response_t rsp = {0};
    	    bt_avrcp_register_notification_commant_t *param = (bt_avrcp_register_notification_commant_t*)buffer;
            rsp.event_id = param->event_id;
            rsp.response_type = BT_AVRCP_RESPONSE_REJECTED;
                
            dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_AVRCP, (const void *)(&(param->handle)));
    	    rsp.parameter_length = 9;
    	    if (dev) {
                rsp.response_type = BT_AVRCP_RESPONSE_INTERIM;
    	        if (BT_AVRCP_EVENT_TRACK_CHANGED == param->event_id) {
        		    rsp.id = 0xFFFFFFFFFFFFFFFF;//default value
                } else if (BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED == param->event_id) {
        		    rsp.status = BT_AVRCP_STATUS_PLAY_STOPPED;
                }
    	    } 
    	    bt_source_srv_report("[source][avrcp]register notification ind event:0x%x\n", param->event_id);		
    	    bt_avrcp_send_register_notification_response(param->handle, &rsp);
            break;
    	}
        
        default:
            break;
    }
    bt_source_srv_report("[source][avrcp]common_hdr[e]-ret: 0x%x\n", ret);

    return ret;
}



