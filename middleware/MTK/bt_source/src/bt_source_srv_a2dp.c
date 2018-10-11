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

#include "bt_source_srv_a2dp.h"
#include "bt_source_srv_avrcp.h"
#include "bt_source_srv_utils.h"
#include "bt_source_srv_cntx.h"
#include "bt_source_srv.h"
#include "bt_platform.h"
#include "bt_a2dp.h"
#include "timers.h"

#define BT_SOURCE_SRV_A2DP_CODEC_MAX_NUM           (1)
#define BT_SOURCE_AVRCP_CONNECT_TIMER_PERIOD       (1000)

#if 0
_Pragma("data_alignment=4") uint8_t bt_a2dp_payload_buf[BT_SOURCE_SRV_A2DP_PAYLOAD_SIZE] = {0};
_Pragma("data_alignment=4") uint8_t bt_a2dp_sbc_share_buf[BT_SOURCE_SRV_SBC_SHARE_SIZE] = {0};
#endif

BT_ALIGNMENT4(
static char bt_a2dp_payload_buf[BT_SOURCE_SRV_A2DP_PAYLOAD_SIZE]
);

BT_ALIGNMENT4(
static char bt_a2dp_sbc_share_buf[BT_SOURCE_SRV_SBC_SHARE_SIZE]
);

static uint32_t packet_num = 0;
static TimerHandle_t xTimer_avrcp_connect = NULL;
static bt_media_handle_t *bt_a2dp_media_handle = NULL;
static bt_a2dp_codec_capability_t g_bt_source_srv_a2dp_codec_list[BT_SOURCE_SRV_A2DP_CODEC_MAX_NUM];
static const bt_codec_sbc_t sbc_cap[1] = {
    {
        25,  /* min_bit_pool */
        75,  /* max_bit_pool */
        0xf, /* block_len: all */
        0xf, /* subband_num: all */
        0x3, /* both snr/loudness */
        0xf, /* sample_rate: all */
        0xf  /* channel_mode: all */
    }
};

extern bt_status_t bt_source_srv_event_callback(bt_source_srv_event_t event_id, void *param);
static void bt_source_srv_a2dp_handle_avrcp_connect(void);
static bt_status_t bt_source_srv_a2dp_close_codec(bt_media_handle_t *sbc_handle);
static bt_status_t bt_source_srv_a2dp_open_codec(bt_a2dp_codec_capability_t *codec_cap);
static void bt_source_srv_a2dp_handle_avrcp_connect_timeout(TimerHandle_t xTimer);

static void BT_A2DP_SRC_MAKE_SBC_CODEC(bt_a2dp_codec_capability_t *codec,
                            bt_a2dp_role_t role,
                            uint8_t min_bit_pool, uint8_t max_bit_pool,
                            uint8_t block_length, uint8_t subband_num,
                            uint8_t alloc_method, uint8_t sample_rate,
                            uint8_t channel_mode)
{
    do {
        codec->type = BT_A2DP_CODEC_SBC;
        codec->sep_type = role;
        codec->length = sizeof(bt_a2dp_sbc_codec_t);
        codec->codec.sbc.channel_mode = (channel_mode & 0x0F);
        codec->codec.sbc.sample_freq = (sample_rate & 0x0F);
        codec->codec.sbc.alloc_method = (alloc_method & 0x03);
        codec->codec.sbc.subbands = (subband_num & 0x03);
        codec->codec.sbc.block_len = (block_length & 0x0F);
        codec->codec.sbc.min_bitpool = (min_bit_pool & 0xFF);
        codec->codec.sbc.max_bitpool = (max_bit_pool & 0xFF);
    } while (0);
}

bt_status_t bt_source_srv_a2dp_get_init_params(bt_a2dp_init_params_t *param)
{
    uint32_t num = 0;
    int32_t ret = BT_STATUS_FAIL;

    if (param) {/** init source sep. */
        BT_A2DP_SRC_MAKE_SBC_CODEC(g_bt_source_srv_a2dp_codec_list + num, BT_A2DP_SOURCE,
                               sbc_cap[0].min_bit_pool, sbc_cap[0].max_bit_pool,
                               sbc_cap[0].block_length, sbc_cap[0].subband_num,
                               sbc_cap[0].alloc_method, sbc_cap[0].sample_rate,
                               sbc_cap[0].channel_mode);
        num++;

        /** fill init params. */
        param->codec_number = num;
        param->codec_list = g_bt_source_srv_a2dp_codec_list;
        ret = BT_STATUS_SUCCESS;
    }
    bt_source_srv_report("[source][a2dp]init_params-ret: %d\n", ret);
    return ret;
}

bt_status_t bt_source_srv_a2dp_connect(bt_bd_addr_t *address)
{
    uint32_t hd = 0;
    int32_t err = 0;
    bt_status_t ret = BT_STATUS_FAIL;
    bt_source_srv_device_t *dev = NULL;

    dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_ADDR, (const void *)address);
    if (dev) {
        if ((dev->conn_bit & BT_SOURCE_SRV_A2DP_CONN_BIT) && (BT_SOURCE_SRV_INVALID_HD != dev->a2dp_hd)) {
            err = -1;
            bt_source_srv_report("[source][a2dp]connect fail: ret:0x%x, errcode: %d\r\n", ret, err);  
            return ret;
        }
        /** unexpected, need handle. */
        err = -2;
    } else {
        dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_UNUSED, NULL);
    }
    
    if (dev) {
        ret = bt_a2dp_connect(&hd, (const bt_bd_addr_t *)address, BT_A2DP_SOURCE);
        if (BT_STATUS_SUCCESS == ret) {/** normal case. */
            dev->a2dp_hd = hd;
            dev->a2dp_role = BT_A2DP_SOURCE;
            bt_source_srv_memcpy(&(dev->dev_addr), address, sizeof(bt_bd_addr_t));
        }
    } else {/** unexpected, no unused device, please check log. */
        err = -3;                
    }
    
    bt_source_srv_report("[source][a2dp]connect: ret: 0x%x, errcode: %d, hd: 0x%x\r\n", ret, err, hd);  
    return ret;
}

bt_status_t bt_source_srv_a2dp_disconnect(uint32_t handle)
{
    int32_t err = 0;
    bt_status_t ret = BT_STATUS_FAIL;
    bt_source_srv_device_t *dev = NULL;

    dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_A2DP, (const void *)(&handle));
    if ((dev) && (dev->conn_bit & BT_SOURCE_SRV_A2DP_CONN_BIT)) {
        ret = bt_a2dp_disconnect(dev->a2dp_hd);
    } else {
        err = -1;
    }
    bt_source_srv_report("[source][a2dp]disconnect-a2dp_hd: 0x%x, ret: 0x%x, errcode: %d\r\n", handle, ret, err);
    return ret;
}

static void bt_source_srv_a2dp_handle_avrcp_connect_timeout(TimerHandle_t xTimer)
{
    bt_source_srv_device_t *dev = NULL;
    bt_bd_addr_t address = {0};
    bt_status_t sta = BT_STATUS_FAIL;

    bt_source_srv_report("[source][a2dp]xTimer_avrcp_connect timeout!\r\n");

    /** avrcp connect. */
    dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_USED, NULL);
    if (dev) {
        bt_source_srv_memcpy((void *)(&(address)),(const void *)(&(dev->dev_addr)), sizeof(bt_bd_addr_t)); 
        bt_source_srv_report("[source][a2dp]xTimer_avrcp_connect timeout active connect: 0x%x\n", dev->conn_bit);
        
        if (!(dev->conn_bit & BT_SOURCE_SRV_AVRCP_CONN_BIT)) {
            sta = bt_source_srv_avrcp_connect((bt_bd_addr_t *)(&(dev->dev_addr)));
            bt_source_srv_report("[source][a2dp]xTimer_avrcp_connect timeout active connect, sta: 0x%x\r\n", sta);
        }
    } 

    if (xTimerIsTimerActive(xTimer_avrcp_connect)) {
        xTimerStop(xTimer_avrcp_connect, 0);
        xTimerDelete((TimerHandle_t)xTimer_avrcp_connect, 0);
    }
}

static void bt_source_srv_a2dp_handle_avrcp_connect(void)
{
    xTimer_avrcp_connect = xTimerCreate("AVRCP_ACTIVE_CONNECT_TIMER",
                                  BT_SOURCE_AVRCP_CONNECT_TIMER_PERIOD / portTICK_PERIOD_MS, pdFALSE,
                                  ( void *)0,
                                  bt_source_srv_a2dp_handle_avrcp_connect_timeout);

    bt_source_srv_report("[source][a2dp]xTimer_avrcp_connect: 0x%x\r\n", xTimer_avrcp_connect);

    if (xTimer_avrcp_connect == NULL) {
        bt_source_srv_report("[source][a2dp]xTimer_avrcp_connect create fail!\r\n");
    } else {
        xTimerStart((TimerHandle_t)xTimer_avrcp_connect, 0);
    }
}

static bt_status_t bt_source_srv_a2dp_handle_connect_cnf(bt_a2dp_connect_cnf_t *conn_cnf)
{
    bt_status_t ret = 0;
    uint32_t hd = 0;
    bt_source_srv_device_t *dev = NULL;
    bt_source_srv_a2dp_connect_t a2dp_connect;
    bt_bd_addr_t address = {0};

    if (conn_cnf->status == BT_STATUS_SUCCESS) {/** a2dp connected. */
        dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_A2DP, (void *)(&(conn_cnf->handle)));
        if (dev) {
            bt_source_srv_memcpy((void *)(&(address)),(const void *)(&(dev->dev_addr)), sizeof(bt_bd_addr_t));
            dev->conn_bit |= BT_SOURCE_SRV_A2DP_CONN_BIT;
            
            dev->a2dp.profile.state = BT_SOURCE_SRV_A2DP_OPEN;            
            memcpy(&(dev->a2dp.codec), (conn_cnf->codec_cap), sizeof(bt_a2dp_codec_capability_t));

            bt_source_srv_report("[source][a2dp]connect_cnf success-con_bit: 0x%x\n", dev->conn_bit);
            if (!(dev->conn_bit & BT_SOURCE_SRV_AVRCP_CONN_BIT)) {
                bt_source_srv_a2dp_handle_avrcp_connect();
                bt_source_srv_report("[source][a2dp]connect_cnf-avrcp_hd: 0x%x\r\n", hd);
            }
        } 
    } else {/** connect fail, so reset the cntx. */
    
        bt_source_srv_report("[source][a2dp]connect_cnf-fail\r\n");
        dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_A2DP, (void *)(&(conn_cnf->handle)));
        if (dev) {
            dev->conn_bit &= ~(BT_SOURCE_SRV_A2DP_CONN_BIT);
            bt_source_srv_reset_a2dp_info(dev);

            if (!(dev->conn_bit)) {
                bt_source_srv_reset_device(dev);
            }
        }
    }
    /** To notify app. */
    {
        a2dp_connect.handle = conn_cnf->handle;
        a2dp_connect.result = conn_cnf->status;
        bt_source_srv_memcpy((void *)(&(a2dp_connect.address)),(const void *)address, sizeof(bt_bd_addr_t));
        bt_source_srv_event_callback(BT_SOURCE_SRV_EVENT_A2DP_CONNECT, (void *)(&a2dp_connect));   
    }
    bt_source_srv_report("[source][a2dp]conn_cnf-handle: 0x%x, ret: 0x%x\r\n", conn_cnf->handle, ret);
    return ret;
}

static bt_status_t bt_source_srv_a2dp_handle_connect_ind(bt_a2dp_connect_ind_t *conn_ind)
{
    bt_source_srv_device_t *dev = NULL;
    bt_status_t ret = 0;

    /** a2dp connected. */
    dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_ADDR, (void *)(conn_ind->address));
    if (dev) {
        ;
    } else {
        dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_UNUSED, NULL);
    }
    if (dev) {
        dev->a2dp_hd = conn_ind->handle;
        dev->a2dp_role = conn_ind->role;
        bt_source_srv_memcpy(&(dev->dev_addr), conn_ind->address, sizeof(bt_bd_addr_t));  
        ret = bt_a2dp_connect_response(conn_ind->handle, true);
    } else {
        ret = bt_a2dp_connect_response(conn_ind->handle, false);
    }
    
    bt_source_srv_report("[source][a2dp]conn_ind-handle: 0x%x, ret: 0x%x\r\n", conn_ind->handle, ret);
    return ret;
}

static bt_status_t bt_source_srv_a2dp_handle_disconnect_cnf(bt_a2dp_disconnect_cnf_t *disconn_cnf)
{
    bt_status_t ret = 0;
    bt_status_t sta = BT_STATUS_SUCCESS;
    bt_source_srv_device_t *dev = NULL;
    bt_source_srv_a2dp_disconnect_t disconn_param;
    bt_bd_addr_t address = {0};

    if (BT_STATUS_SUCCESS == disconn_cnf->status) {
        dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_A2DP, (void *)(&(disconn_cnf->handle)));
        if (dev) {
            dev->conn_bit &= ~(BT_SOURCE_SRV_A2DP_CONN_BIT);
            if ((dev->a2dp.profile.state == BT_SOURCE_SRV_A2DP_READY) ||
                (dev->a2dp.profile.state == BT_SOURCE_SRV_A2DP_PLAYING)) {
                sta = bt_source_srv_a2dp_close_codec(dev->a2dp.profile.med_handle);
                if (BT_STATUS_SUCCESS == sta) {
                    bt_source_srv_reset_a2dp_info(dev);
                }
                bt_source_srv_report("[source][a2dp]disconnect_cnf-ret: 0x%x, sta: 0x%x\r\n", ret, sta);
            }
            
            bt_source_srv_report("[source][a2dp]disconnect_cnf-con_bit: 0x%x\n", dev->conn_bit);
            if (!(dev->conn_bit)) {
                bt_source_srv_reset_device(dev);
            }
        }
    }
    /** To notify app. */
    {
        disconn_param.handle = disconn_cnf->handle;
        disconn_param.result = disconn_cnf->status;
        bt_source_srv_memcpy(&(disconn_param.address), &address, sizeof(bt_bd_addr_t));
        bt_source_srv_event_callback(BT_SOURCE_SRV_EVENT_A2DP_DISCONNECT, (void *)(&disconn_param));
    }
    bt_source_srv_report("[source][a2dp]disconn_cnf-handle: 0x%x, ret: 0x%x\r\n", disconn_cnf->handle, ret); 
    return ret;
}

static int32_t bt_source_srv_a2dp_handle_disconnect_ind(bt_a2dp_disconnect_ind_t *disconn_ind)
{
    int32_t ret = 0;
    bt_status_t sta = 0;
    bt_source_srv_device_t *dev = NULL;
    bt_source_srv_a2dp_disconnect_t disconn_param;
    bt_bd_addr_t address = {0};
   
    dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_A2DP, (void *)(&(disconn_ind->handle)));
    if (dev) {
        bt_source_srv_memcpy(&address, &(dev->dev_addr), sizeof(bt_bd_addr_t));
        if (dev->a2dp_hd == disconn_ind->handle) {
            if ((dev->a2dp.profile.state == BT_SOURCE_SRV_A2DP_READY) ||
                (dev->a2dp.profile.state == BT_SOURCE_SRV_A2DP_PLAYING)){
                sta = bt_source_srv_a2dp_close_codec(dev->a2dp.profile.med_handle);
                bt_source_srv_report("[sink][a2dp]disconnect_ind-sta: 0x%x\r\n", sta);
            }
            dev->conn_bit &= ~(BT_SOURCE_SRV_A2DP_CONN_BIT);
            bt_source_srv_reset_a2dp_info(dev);       
        }
        if (!(dev->conn_bit)) {
            bt_source_srv_reset_device(dev);
        }
    }
    
    /** To notify app. */
    {
        disconn_param.handle = disconn_ind->handle;
        disconn_param.result = 0;
        bt_source_srv_memcpy(&(disconn_param.address), &address, sizeof(bt_bd_addr_t));
        bt_source_srv_event_callback(BT_SOURCE_SRV_EVENT_A2DP_DISCONNECT, (void *)(&disconn_param));
    }
    bt_source_srv_report("[source][a2dp]disconn_ind-handle: 0x%x\r\n", disconn_ind->handle);
    return ret;
}

static bt_status_t bt_source_srv_a2dp_handle_start_streaming_cnf(bt_a2dp_start_streaming_cnf_t *start_cnf)
{
    bt_status_t ret = BT_STATUS_FAIL;
    bt_source_srv_device_t *dev = NULL;
    bt_source_srv_a2dp_start_cnf_t play_result;

    bt_source_srv_report("[source][a2dp]start_cnf-handle: 0x%x\n", start_cnf->handle);

    if (BT_STATUS_SUCCESS == start_cnf->status) {
        dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_A2DP, (const void *)(&(start_cnf->handle)));
        if (dev) {
            if (BT_SOURCE_SRV_A2DP_OPEN == dev->a2dp.profile.state) {
                dev->a2dp.profile.state = BT_SOURCE_SRV_A2DP_STREAMING;
                ret = bt_source_srv_a2dp_open_codec(start_cnf->codec_cap);
                if (ret == BT_STATUS_SUCCESS) {/** open and play codec success. */
                    dev->a2dp.profile.timestamp = 0;
                    dev->a2dp.profile.state = BT_SOURCE_SRV_A2DP_READY;   
                    dev->a2dp.profile.med_handle = bt_a2dp_media_handle;
                    bt_source_srv_memcpy(&(dev->a2dp.codec), (start_cnf->codec_cap), sizeof(bt_a2dp_codec_capability_t));
                    bt_source_srv_report("[source][a2dp]start_cnf-open success,state: %d\n", dev->a2dp.profile.state);
                } else {
                    bt_a2dp_suspend_streaming(dev->a2dp_hd);    
                }
            } 
        }
    } else {
        ret = start_cnf->status;
    }
    /** To notify app. */
    {
        play_result.handle = start_cnf->handle;
        play_result.result = ret;
        bt_source_srv_event_callback(BT_SOURCE_SRV_EVENT_A2DP_START_CNF, (void *)(&play_result));
    }
    bt_source_srv_report("[source][a2dp]start_cnf-handle: 0x%x, ret: 0x%x\r\n", start_cnf->handle, ret);  
    return ret;
}

static bt_status_t bt_source_srv_a2dp_handle_start_streaming_ind(bt_a2dp_start_streaming_ind_t *start_ind)
{
    int32_t err = 0;
    bt_status_t ret = BT_STATUS_FAIL;
    bt_source_srv_device_t *dev = NULL;
    bt_source_srv_a2dp_start_ind_t play_result;

    bt_source_srv_report("[source][a2dp]start_ind-handle: 0x%x\n", start_ind->handle);

    dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_A2DP, (const void *)(&(start_ind->handle)));
    if (dev) {
        if (BT_SOURCE_SRV_A2DP_OPEN == dev->a2dp.profile.state) {
            dev->a2dp.profile.state = BT_SOURCE_SRV_A2DP_STREAMING;
            ret = bt_source_srv_a2dp_open_codec(start_ind->codec_cap);
            if (BT_STATUS_SUCCESS == ret) {/** open and play codec success. */
                dev->a2dp.profile.timestamp = 0;
                dev->a2dp.profile.state = BT_SOURCE_SRV_A2DP_READY;   
                dev->a2dp.profile.med_handle = bt_a2dp_media_handle;
                bt_source_srv_memcpy(&(dev->a2dp.codec), (start_ind->codec_cap), sizeof(bt_a2dp_codec_capability_t));
                bt_source_srv_report("[source][a2dp]start_ind-open success,state: %d\n", dev->a2dp.profile.state);

                ret = bt_a2dp_start_streaming_response(start_ind->handle, true);
                if (BT_STATUS_SUCCESS == ret) {/** To notify app. */
                    play_result.handle = start_ind->handle;
                    bt_source_srv_event_callback(BT_SOURCE_SRV_EVENT_A2DP_START_IND, (void *)(&play_result));
                }
            } else {
                err = -100;
                dev->a2dp.profile.state = BT_SOURCE_SRV_A2DP_OPEN;
                ret = bt_a2dp_start_streaming_response(start_ind->handle, false);
            }
        } 
    } else {
        err = -101;
        ret = bt_a2dp_start_streaming_response(start_ind->handle, false);
    }
    bt_source_srv_report("[source][a2dp]start_ind-handle: 0x%x, error: %d, ret: 0x%x\r\n", start_ind->handle, err, ret);  
    return ret;
}

static bt_status_t bt_source_srv_a2dp_handle_suspend_streaming_cnf(bt_a2dp_suspend_streaming_cnf_t *suspend_cnf)
{
    bt_status_t ret = 0;
    bt_source_srv_device_t *dev = NULL;
    bt_source_srv_a2dp_stop_cnf_t stop_result;
    bool no2notify = false;

    bt_source_srv_report("[source][a2dp]suspend_cnf-handle: 0x%x\r\n", suspend_cnf->handle);

    if (suspend_cnf->status == BT_STATUS_SUCCESS) {
        dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_A2DP, (void *)(&(suspend_cnf->handle)));
        if ((dev) && (dev->conn_bit & BT_SOURCE_SRV_A2DP_CONN_BIT)) {
            if ((dev->a2dp.profile.state == BT_SOURCE_SRV_A2DP_PLAYING) ||
                (dev->a2dp.profile.state == BT_SOURCE_SRV_A2DP_READY)) {
                dev->a2dp.profile.state = BT_SOURCE_SRV_A2DP_OPEN;
                dev->avrcp_play_status = BT_AVRCP_EVENT_MEDIA_PAUSED;
                
                bt_source_srv_report("[source][a2dp]suspend_cnf-state: 0x%x\r\n", dev->a2dp.profile.state);
            } else if (dev->a2dp.profile.state == BT_SOURCE_SRV_A2DP_STREAMING) {
                dev->a2dp.profile.state = BT_SOURCE_SRV_A2DP_OPEN;
                no2notify = true;
            }
        }
    } else {
        ret = suspend_cnf->status;
    }
    
    /** To notify app. */
    if (!no2notify)
    {
        stop_result.handle = suspend_cnf->handle;
        stop_result.result = ret;
        bt_source_srv_event_callback(BT_SOURCE_SRV_EVENT_A2DP_STOP_CNF, (void *)(&stop_result));
    }
    bt_source_srv_report("[source][a2dp]suspend_cnf-handle: 0x%x, ret: 0x%x\r\n", suspend_cnf->handle, ret);
    return ret;
}

static bt_status_t bt_source_srv_a2dp_handle_suspend_streaming_ind(bt_a2dp_suspend_streaming_ind_t *suspend_ind)
{
    bt_status_t ret = 0;
    bt_source_srv_device_t *dev = NULL;
    bt_source_srv_a2dp_stop_ind_t stop_result;
    
    dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_A2DP, (void *)(&(suspend_ind->handle)));
    
    if ((dev) && (dev->conn_bit & BT_SOURCE_SRV_A2DP_CONN_BIT)) {
        if ((dev->a2dp.profile.state == BT_SOURCE_SRV_A2DP_PLAYING) ||
            (dev->a2dp.profile.state == BT_SOURCE_SRV_A2DP_READY)) {
            dev->a2dp.profile.state = BT_SOURCE_SRV_A2DP_OPEN;
            dev->avrcp_play_status = BT_AVRCP_EVENT_MEDIA_PAUSED;
            ret = bt_source_srv_a2dp_close_codec(dev->a2dp.profile.med_handle);
            if (BT_STATUS_SUCCESS == ret) { 
                bt_source_srv_report("[source][a2dp]suspend_ind-close codec success\r\n");
                dev->a2dp.profile.timestamp = 0;
                dev->a2dp.profile.med_handle = NULL;
            } 
        } else if (dev->a2dp.profile.state == BT_SOURCE_SRV_A2DP_STREAMING) {
                dev->a2dp.profile.state = BT_SOURCE_SRV_A2DP_OPEN;
        }
        ret = bt_a2dp_suspend_streaming_response(suspend_ind->handle, true);
        /** To notify app. */
        if (BT_STATUS_SUCCESS == ret)
        {
            stop_result.handle = suspend_ind->handle;
            bt_source_srv_event_callback(BT_SOURCE_SRV_EVENT_A2DP_STOP_IND, (void *)(&stop_result));
        }
    } else {
        ret = bt_a2dp_suspend_streaming_response(suspend_ind->handle, false);
    }
    bt_source_srv_report("[source][a2dp]suspend_ind-handle: 0x%x, ret: 0x%x\r\n", suspend_ind, ret);  
    return ret;
}

static bt_status_t bt_source_srv_a2dp_handle_reconfigure_cnf(bt_a2dp_reconfigure_cnf_t *reconfigure_cnf)
{
    bt_status_t ret = 0;
    bt_source_srv_device_t *dev = NULL;

    if (BT_STATUS_SUCCESS == reconfigure_cnf->status) {/** a2dp connected. */
        dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_A2DP, (const void *)(&(reconfigure_cnf->handle)));
        if (dev) {
            bt_source_srv_report("[source][a2dp]reconfig_cnf-handle: 0x%x\n", reconfigure_cnf->handle);
        } 
    }
    return ret;
}

static bt_status_t bt_source_srv_a2dp_handle_reconfigure_ind(bt_a2dp_reconfigure_ind_t *reconfigure_ind)
{
    bt_status_t ret = BT_STATUS_FAIL;
    bt_source_srv_device_t *dev = NULL;

    dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_A2DP, (const void *)(&(reconfigure_ind->handle)));
    if (dev) {
        bt_source_srv_memcpy(&(dev->a2dp.codec), &(reconfigure_ind->codec_cap), sizeof(bt_a2dp_codec_capability_t));
        ret = bt_a2dp_reconfigure_response(reconfigure_ind->handle, true);
    } else {
        ret = bt_a2dp_reconfigure_response(reconfigure_ind->handle, false);
    }
    return ret;
}

bt_status_t bt_source_srv_a2dp_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_status_t ret = 0;
    bt_source_srv_report("[source][a2dp]common_callback[s]-msg: 0x%x, status: 0x%x\r\n", msg, status);

    switch (msg) {
        case BT_A2DP_CONNECT_CNF: {
            bt_a2dp_connect_cnf_t *conn_cnf = (bt_a2dp_connect_cnf_t *)buffer;
            ret = bt_source_srv_a2dp_handle_connect_cnf(conn_cnf);
            break;
        }
        
        case BT_A2DP_CONNECT_IND: {
            bt_a2dp_connect_ind_t *conn_ind = (bt_a2dp_connect_ind_t *)buffer;
            ret = bt_source_srv_a2dp_handle_connect_ind(conn_ind);
            break;
        }
        
        case BT_A2DP_DISCONNECT_CNF: {
            bt_a2dp_disconnect_cnf_t *disconn_cnf = (bt_a2dp_disconnect_cnf_t *)buffer;
            ret = bt_source_srv_a2dp_handle_disconnect_cnf(disconn_cnf);
            break;
        }

        case BT_A2DP_DISCONNECT_IND: {
            bt_a2dp_disconnect_ind_t *disconn_ind = (bt_a2dp_disconnect_ind_t *)buffer;
            ret = bt_source_srv_a2dp_handle_disconnect_ind(disconn_ind);
            break;
        }

        case BT_A2DP_START_STREAMING_CNF: {
            bt_a2dp_start_streaming_cnf_t *start_cnf = (bt_a2dp_start_streaming_cnf_t *)buffer;
            ret = bt_source_srv_a2dp_handle_start_streaming_cnf(start_cnf);
            break;
        }

        case BT_A2DP_START_STREAMING_IND: {
            bt_a2dp_start_streaming_ind_t *start_ind = (bt_a2dp_start_streaming_ind_t *)buffer;
            bt_source_srv_report("[source][a2dp]start_ind(comm_bk)--buff: 0x%x, cap: 0x%x\n", buffer, start_ind->codec_cap);

            if (start_ind->codec_cap->type == BT_A2DP_CODEC_SBC) {
                bt_source_srv_report("[source][a2dp]start_ind(comm_bk_sbc)--hd: 0x%x, type: %d, sep_type: %d, len: %d, 1: %d, 2: %d, 3: %d, 4: %d, 5: %d, 6: %d, 7: %d\r\n",
                                   start_ind->handle,
                                   start_ind->codec_cap->type,
                                   start_ind->codec_cap->sep_type,
                                   start_ind->codec_cap->length,
                                   start_ind->codec_cap->codec.sbc.channel_mode,
                                   start_ind->codec_cap->codec.sbc.sample_freq,
                                   start_ind->codec_cap->codec.sbc.alloc_method,
                                   start_ind->codec_cap->codec.sbc.subbands,
                                   start_ind->codec_cap->codec.sbc.block_len,
                                   start_ind->codec_cap->codec.sbc.min_bitpool,
                                   start_ind->codec_cap->codec.sbc.max_bitpool);
            } else if (start_ind->codec_cap->type == BT_A2DP_CODEC_AAC) {
                bt_source_srv_report("[source][a2dp]start_ind(comm_bk_aac)--hd: 0x%x, type: %d, sep_type: %d, len: %d, 1: %d, 2: %d, 3: %d, 4: %d, 5: %d, 6: %d, 7: %d, 8: %d, 9: %d\r\n",
                                   start_ind->handle,
                                   start_ind->codec_cap->type,
                                   start_ind->codec_cap->sep_type,
                                   start_ind->codec_cap->length,
                                   start_ind->codec_cap->codec.aac.object_type,
                                   start_ind->codec_cap->codec.aac.freq_h,
                                   start_ind->codec_cap->codec.aac.reserved,
                                   start_ind->codec_cap->codec.aac.channels,
                                   start_ind->codec_cap->codec.aac.freq_l,
                                   start_ind->codec_cap->codec.aac.br_h,
                                   start_ind->codec_cap->codec.aac.vbr,
                                   start_ind->codec_cap->codec.aac.br_m,
                                   start_ind->codec_cap->codec.aac.br_l);
            }
            ret = bt_source_srv_a2dp_handle_start_streaming_ind(start_ind);
            break;
        }

        case BT_A2DP_SUSPEND_STREAMING_CNF: {
            bt_a2dp_suspend_streaming_cnf_t *suspend_cnf = (bt_a2dp_suspend_streaming_cnf_t *)buffer;
            ret = bt_source_srv_a2dp_handle_suspend_streaming_cnf(suspend_cnf);
            break;
        }

        case BT_A2DP_SUSPEND_STREAMING_IND: {
            bt_a2dp_suspend_streaming_ind_t *suspend_ind = (bt_a2dp_suspend_streaming_ind_t *)buffer;       
            ret = bt_source_srv_a2dp_handle_suspend_streaming_ind(suspend_ind);
            break;
        }

        case BT_A2DP_RECONFIGURE_CNF: {
            bt_a2dp_reconfigure_cnf_t *reconfigure_cnf = (bt_a2dp_reconfigure_cnf_t *)buffer;
            ret = bt_source_srv_a2dp_handle_reconfigure_cnf(reconfigure_cnf);
            break;
        }

        case BT_A2DP_RECONFIGURE_IND: {
            bt_a2dp_reconfigure_ind_t *reconfigure_ind = (bt_a2dp_reconfigure_ind_t *)buffer;
            ret = bt_source_srv_a2dp_handle_reconfigure_ind(reconfigure_ind);
            break;
        }

        default:
            break;
    }
    bt_source_srv_report("[source][a2dp]common_callback[s]-ret: 0x%x\r\n", ret);
    return ret;
}

static void bt_source_srv_a2dp_sbc_callback(bt_media_handle_t *handle, bt_codec_media_event_t event)
{
    bt_status_t status = BT_STATUS_FAIL;
    uint32_t sample_count = 0;
    bt_source_srv_device_t *dev = NULL;

    switch (event) {
        case BT_CODEC_MEDIA_GET_PAYLOAD:{
            uint32_t actual_size = 0;
            uint32_t mtu_size = 0;

            dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_USED, NULL);
            if ((dev) && BT_SOURCE_SRV_INVALID_HD != dev->a2dp_hd) {
                bt_source_srv_memset((void *)(&bt_a2dp_payload_buf[0]), 0x00, BT_SOURCE_SRV_A2DP_PAYLOAD_SIZE);
                mtu_size = bt_a2dp_get_mtu_size(dev->a2dp_hd);
                actual_size = handle->get_payload(handle, (uint8_t *)(&bt_a2dp_payload_buf[0]), mtu_size, &sample_count);
                //bt_source_srv_report("[source][a2dp]SBC_CODEC_MEDIA_GET_PAYLOAD actual_size :0x%02x\r\n", actual_size);

                if (actual_size > mtu_size) {   
                    printf("[source][a2dp]Big payload data, A2DP send fail !\n");
                    //bt_source_srv_assert(0);
                } else {
                        packet_num++;
                        //bt_source_srv_report("[source][a2dp]SBC_CODEC_MEDIA_GET_PAYLOAD time1 :%d, packet_num: %d\r\n", bt_timer_get_current_tick(), packet_num);                
                        status = bt_a2dp_send_media_data(dev->a2dp_hd, (uint8_t *)(&bt_a2dp_payload_buf[0]), actual_size, (dev->a2dp.profile.timestamp));
                        //bt_source_srv_report("[source][a2dp]SBC_CODEC_MEDIA_GET_PAYLOAD time2 :%d\r\n", bt_timer_get_current_tick());

#if 0
                        /** only for debug. */
                        if (status == BT_STATUS_OUT_OF_MEMORY) {  
                            printf("[source][a2dp]TX OOM, A2DP send fail !\n");
                            //bt_source_srv_assert(0);
                        } else if (BT_STATUS_SUCCESS == status) {
                            dev->a2dp.profile.timestamp += sample_count;
                            dev->a2dp.profile.state = BT_SOURCE_SRV_A2DP_PLAYING;
                            dev->avrcp_play_status = BT_AVRCP_EVENT_MEDIA_PLAYING;
                            //bt_source_srv_report("[source][a2dp]sbc-cb: payload_size:0x%02x,actrual_size:0x%02x, sample_count:0x%02x \r\n", mtu_size, actual_size, dev->a2dp.profile.timestamp);
                            handle->get_payload_done(handle);
                        }
#endif
                    if (BT_STATUS_FAIL != status) {
                        if (BT_STATUS_OUT_OF_MEMORY == status) {
                            bt_source_srv_report("[source][a2dp]send data fail,OOM!\r\n");                                           
                        }
                        dev->a2dp.profile.timestamp += sample_count;
                        dev->a2dp.profile.state = BT_SOURCE_SRV_A2DP_PLAYING;
                        dev->avrcp_play_status = BT_AVRCP_EVENT_MEDIA_PLAYING;
                        handle->get_payload_done(handle);
                    }
                }
            }
            break;
        } 
        
        case BT_CODEC_MEDIA_REQUEST:
        case BT_CODEC_MEDIA_UNDERFLOW:  
            break;

        default:
            break;
    }
}

bt_status_t bt_source_srv_a2dp_start(uint32_t handle)
{
    int32_t err = 0;
    bt_status_t ret = BT_STATUS_FAIL;
    bt_source_srv_device_t *dev = NULL;

    
    dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_A2DP, (const void *)(&handle));
    if ((dev) && (dev->conn_bit & BT_SOURCE_SRV_A2DP_CONN_BIT)) {
        if (BT_SOURCE_SRV_A2DP_OPEN == dev->a2dp.profile.state) {
            ret = bt_a2dp_start_streaming(handle);
            if (BT_STATUS_OUT_OF_MEMORY == ret) {
                //bt_source_srv_assert(0);
                bt_source_srv_report("[source][a2dp]start streaming fail,OOM!\r\n");
            }
        } else {/** state wrong. */
            err = -103;
        }
    } else {/** please help to connect first. */
        err = -104;
    }
    bt_source_srv_report("[source][a2dp]start-hd: 0x%x, ret: 0x%x, err: %d\r\n", handle, ret, err); 
    return ret;
}

static void BT_A2DP_CONVERT_SBC_CODEC(bt_codec_capability_t *dst_codec,
                               bt_a2dp_codec_capability_t *src_codec)
{
    dst_codec->type = BT_A2DP_CODEC_SBC;
    dst_codec->codec.sbc.min_bit_pool = src_codec->codec.sbc.min_bitpool;
    dst_codec->codec.sbc.max_bit_pool = src_codec->codec.sbc.max_bitpool;
    dst_codec->codec.sbc.block_length = src_codec->codec.sbc.block_len;
    dst_codec->codec.sbc.subband_num = src_codec->codec.sbc.subbands;
    dst_codec->codec.sbc.alloc_method = src_codec->codec.sbc.alloc_method;
    dst_codec->codec.sbc.sample_rate = src_codec->codec.sbc.sample_freq;
    dst_codec->codec.sbc.channel_mode = src_codec->codec.sbc.channel_mode;
    bt_source_srv_report("[source][a2dp]CONVERT_SBC--min_pool: %d, max_pool: %d, b_len: %d, sub_num: %d, all_met: %d, samp_rate: %d, ch_m: %d\n",
                       dst_codec->codec.sbc.min_bit_pool, dst_codec->codec.sbc.max_bit_pool,
                       dst_codec->codec.sbc.block_length, dst_codec->codec.sbc.subband_num,
                       dst_codec->codec.sbc.alloc_method, dst_codec->codec.sbc.sample_rate,
                       dst_codec->codec.sbc.channel_mode);
}

static bt_status_t bt_source_srv_a2dp_open_codec(bt_a2dp_codec_capability_t *codec_cap)
{
    uint32_t total_size = 0;
    uint8_t *temp_buf = NULL;
    bt_status_t ret = BT_STATUS_FAIL;
    bt_media_handle_t *sbc_handle;
    bt_codec_a2dp_audio_t aud_cap;

    bt_source_srv_memset(&aud_cap, 0x00, sizeof(bt_codec_a2dp_audio_t));
    aud_cap.role = BT_A2DP_SOURCE;
    BT_A2DP_CONVERT_SBC_CODEC(&(aud_cap.codec_cap), codec_cap);
    
    sbc_handle = bt_codec_a2dp_open(bt_source_srv_a2dp_sbc_callback, &aud_cap);
    bt_source_srv_assert(sbc_handle);
    bt_source_srv_report("[source][a2dp]open codec success!\r\n");
    
    bt_a2dp_media_handle = sbc_handle;
    total_size = sbc_handle->query_memory_size(sbc_handle);
    bt_source_srv_report("[source][a2dp]total_size = %d\r\n", total_size);
    if ( total_size <= BT_SOURCE_SRV_SBC_SHARE_SIZE ) {
        temp_buf = (uint8_t *)(&bt_a2dp_sbc_share_buf[0]);
    } else {/** only for debug. */
        bt_source_srv_report("[source][a2dp]have not enough heap size!\r\n");
        //bt_source_srv_assert(0);
        return ret;
    }
    sbc_handle->set_buffer(sbc_handle, temp_buf, total_size);
    ret = sbc_handle->play(sbc_handle);
    if (BT_STATUS_SUCCESS != ret) {
        bt_codec_a2dp_close(sbc_handle);
        bt_a2dp_media_handle = NULL;
        packet_num = 0;
    }
    bt_source_srv_report("[source][a2dp]Open sbc driver--ret: 0x%x\r\n", ret);
    return ret;
}

bt_status_t bt_source_srv_a2dp_stop(uint32_t handle)
{
    bt_status_t ret = BT_STATUS_FAIL;
    bt_source_srv_device_t *dev = NULL;

    dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_A2DP, (const void *)(&handle));
    if ((dev) && (dev->conn_bit & BT_SOURCE_SRV_A2DP_CONN_BIT)) {
        bt_source_srv_report("[source][a2dp]stop-state: %d\r\n", dev->a2dp.profile.state); 
        if  ((dev->a2dp.profile.state == BT_SOURCE_SRV_A2DP_PLAYING) ||
            (dev->a2dp.profile.state == BT_SOURCE_SRV_A2DP_READY)) {
            ret = bt_source_srv_a2dp_close_codec(dev->a2dp.profile.med_handle);
            if (BT_STATUS_SUCCESS == ret) { 
                dev->a2dp.profile.timestamp = 0;
                dev->a2dp.profile.med_handle = NULL;
                ret = bt_a2dp_suspend_streaming(handle);    
            }
        }
    }
    bt_source_srv_report("[source][a2dp]stop-hd: 0x%x, ret: 0x%x\r\n", handle, ret); 
    
    return ret;
}

static bt_status_t bt_source_srv_a2dp_close_codec(bt_media_handle_t *sbc_handle)
{
    bt_status_t ret = BT_STATUS_FAIL;
    if (sbc_handle) {
        ret = sbc_handle->stop(sbc_handle);
        if (BT_STATUS_SUCCESS == ret) {
            ret = bt_codec_a2dp_close(sbc_handle);
            bt_a2dp_media_handle = NULL;
            packet_num = 0;
        }
    }
    bt_source_srv_report("[source][a2dp]close-codec: ret: 0x%x\r\n", ret);
    return ret;
}

bt_status_t bt_source_srv_a2dp_reconfig_stream(uint32_t handle, bt_a2dp_codec_capability_t *codec_cap)
{
    int32_t err = 0;
    bt_status_t ret = BT_STATUS_FAIL;
    bt_source_srv_device_t *dev = NULL;

    dev = bt_source_srv_get_device(BT_SOURCE_SRV_DEVICE_A2DP, (const void *)(&handle));
    if ((dev) && (dev->conn_bit & BT_SOURCE_SRV_A2DP_CONN_BIT)) {
        if (dev->a2dp.profile.state == BT_SOURCE_SRV_A2DP_OPEN) {
            ret = bt_a2dp_reconfigure_stream(handle, codec_cap);
            if (BT_STATUS_SUCCESS == ret) {
                bt_source_srv_memcpy(&(dev->a2dp.codec), codec_cap, sizeof(bt_a2dp_codec_capability_t));
            }
        } else {/** Error State. */
            err = -100;
        }
    } else {
        err = -101;
    }
    
    bt_source_srv_report("[source][a2dp]reconfig-req: ret: 0x%x, errcode: %d\r\n", ret, err);
    return ret;
}



