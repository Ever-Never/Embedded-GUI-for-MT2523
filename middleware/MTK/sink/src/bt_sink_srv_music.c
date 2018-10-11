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

#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_music.h"
#include "bt_sink_srv_avrcp.h"
#include "bt_sink_srv_ami.h"
#include "bt_sink_srv_a2dp.h"
#include "bt_sink_srv_aws_a2dp.h"
#include "audio_src_srv.h"
#include "bt_sink_srv_media_mgr.h"
#include "bt_sink_srv_audio_sync.h"


bt_sink_srv_music_context_t g_bt_sink_srv_cntx;

uint32_t sink_loc_play_nclk;
uint32_t sink_loc_play_nclk_intra;

bool g_sink_music_log_disable = false;

void bt_sink_srv_music_init(void)
{
    int32_t i = 0;

    if (g_bt_sink_srv_cntx.a2dp_aid != 0) {
        return;
    }

    memset(&g_bt_sink_srv_cntx, 0x00, sizeof(bt_sink_srv_music_context_t));

    for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
        /* Init invalid a2dp & avrcp & aws handle */
        g_bt_sink_srv_cntx.sink_dev[i].a2dp_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
        g_bt_sink_srv_cntx.sink_dev[i].avrcp_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
        g_bt_sink_srv_cntx.sink_dev[i].role = BT_A2DP_INVALID_ROLE;
    #ifdef __BT_AWS_A2DP_SUPPORT__
        g_bt_sink_srv_cntx.sink_dev[i].aws_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
        g_bt_sink_srv_cntx.sink_dev[i].aws_role = BT_AWS_ROLE_NONE;
    #endif
    }
    g_bt_sink_srv_cntx.a2dp_aid = BT_SINK_SRV_INVALID_AID;
    g_bt_sink_srv_cntx.aws_aid = BT_SINK_SRV_INVALID_AID;
    g_bt_sink_srv_cntx.gpt_port = HAL_GPT_MAX_PORT;
    g_bt_sink_srv_cntx.vol_lev = AUD_VOL_OUT_LEVEL4;

    /* Construct A2DP pseudo handle */
    bt_sink_srv_a2dp_create_pse_handle();
    /* Construct AWS A2DP pseudo handle */
    #ifdef __BT_AWS_A2DP_SUPPORT__
    bt_sink_srv_aws_a2dp_create_pse_handle();
    #endif
    bt_sink_srv_report("[sink][music]init-EVT_START: 0x%x", BT_SINK_SRV_MUSIC_EVT_START);
}


bt_sink_srv_music_context_t *bt_sink_srv_music_get_context(void)
{
    return &g_bt_sink_srv_cntx;
}


bt_sink_srv_music_device_t *bt_sink_srv_music_get_device(bt_sink_srv_music_device_type_t type, const void *param)
{
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *ctx = NULL;
    bt_bd_addr_t *dev_addr = NULL;
    int32_t i = 0;
    uint32_t *p_hd = NULL;
    #ifdef __BT_AWS_SUPPORT__
    audio_src_srv_pseudo_device_t pse_type = AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP;
    #endif
    audio_src_srv_handle_t *pse_hd = NULL;
    bool aws_exist = false;

    ctx = bt_sink_srv_music_get_context();

    switch (type) {
        case BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD: {
            p_hd = (uint32_t *)param;

            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if (ctx->sink_dev[i].a2dp_hd != BT_SINK_SRV_MUSIC_INVALID_HD
                        && ctx->sink_dev[i].a2dp_hd == *p_hd) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_AVRCP_HD: {
            p_hd = (uint32_t *)param;

            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if (ctx->sink_dev[i].avrcp_hd != BT_SINK_SRV_MUSIC_INVALID_HD
                        && ctx->sink_dev[i].avrcp_hd == *p_hd) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_ADDR_A2DP: {
            dev_addr = (bt_bd_addr_t *)param;

            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if ((ctx->sink_dev[i].a2dp_hd != BT_SINK_SRV_MUSIC_INVALID_HD) &&
                    (memcmp(dev_addr, &(ctx->sink_dev[i].dev_addr), sizeof(bt_bd_addr_t)) == 0)) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_ADDR_AVRCP: {
            dev_addr = (bt_bd_addr_t *)param;

            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if ((ctx->sink_dev[i].avrcp_hd != BT_SINK_SRV_MUSIC_INVALID_HD) &&
                    (memcmp(dev_addr, &(ctx->sink_dev[i].dev_addr), sizeof(bt_bd_addr_t)) == 0)) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

    #ifdef __BT_AWS_SUPPORT__
        case BT_SINK_SRV_MUSIC_DEVICE_ADDR_AWS_MP3: {
            dev_addr = (bt_bd_addr_t *)param;

            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if ((ctx->sink_dev[i].aws_hd != BT_SINK_SRV_MUSIC_INVALID_HD) &&
                    ((ctx->sink_dev[i].mp3->audio_src) && (ctx->sink_dev[i].mp3->audio_src->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_MP3)) &&
                    (memcmp(dev_addr, &(ctx->sink_dev[i].dev_addr), sizeof(bt_bd_addr_t)) == 0)) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }

            break;
        }
    #endif
        case BT_SINK_SRV_MUSIC_DEVICE_UNUSED: {
            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if ((ctx->sink_dev[i].a2dp_hd == BT_SINK_SRV_MUSIC_INVALID_HD)
                    && (ctx->sink_dev[i].avrcp_hd == BT_SINK_SRV_MUSIC_INVALID_HD)
            #ifdef __BT_AWS_SUPPORT__
                    && (ctx->sink_dev[i].aws_hd == BT_SINK_SRV_MUSIC_INVALID_HD)
            #endif
                    && (ctx->sink_dev[i].mp3 == NULL)
                ) {
                    dev = &(ctx->sink_dev[i]);
                    ctx->latest_dev = dev;
                    break;
                }
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_USED: {
            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if (ctx->sink_dev[i].a2dp_hd != BT_SINK_SRV_MUSIC_INVALID_HD) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_LATEST: {
            dev = ctx->latest_dev;
            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_INTERRUPT: {
            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if (ctx->sink_dev[i].flag & BT_SINK_SRV_MUSIC_FLAG_A2DP_INTERRUPT) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_FOCUS: {
            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if (ctx->sink_dev[i].conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT) {
                    aws_exist = true;
                    break;
                }
            }

            /* find SP dev information */
            if (aws_exist) {
                for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                    if ((ctx->sink_dev[i].conn_bit & BT_SINK_SRV_MUSIC_A2DP_CONN_BIT) &&
                        (ctx->sink_dev[i].conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
                        dev = &(ctx->sink_dev[i]);
                        break;
                    }
                }
            } else {
                /* if there are no streaming dev, the latest conneced dev will be
                the focus dev */
                if (ctx->focus_dev) {
                    dev = ctx->focus_dev;
                } else {
                    dev = ctx->latest_dev;
                }
            }
            break;
        }
    #ifdef __BT_AWS_SUPPORT__
        case BT_SINK_SRV_MUSIC_DEVICE_AWS_A2DP_HD: {
            p_hd = (uint32_t *)param;

            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if ((ctx->sink_dev[i].aws_hd != BT_SINK_SRV_MUSIC_INVALID_HD) &&
                    ((ctx->sink_dev[i].handle) && (ctx->sink_dev[i].handle->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP)) &&
                    (ctx->sink_dev[i].aws_hd == *p_hd)) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_AWS_MP3_HD: {
            p_hd = (uint32_t *)param;
        
            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if ((ctx->sink_dev[i].aws_hd != BT_SINK_SRV_MUSIC_INVALID_HD) &&
                    ((ctx->sink_dev[i].mp3->audio_src) && (ctx->sink_dev[i].mp3->audio_src->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_MP3)) &&
                    (ctx->sink_dev[i].aws_hd == *p_hd)) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }
        
            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_AWS: {
            uint32_t tmp = (uint32_t)param;
            pse_type = (audio_src_srv_pseudo_device_t)tmp;
            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if ((ctx->sink_dev[i].aws_hd != BT_SINK_SRV_MUSIC_INVALID_HD) &&
                    (ctx->sink_dev[i].conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT) &&
                    ((ctx->sink_dev[i].handle) && (ctx->sink_dev[i].handle->type == pse_type))) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }

            break;
        }
    #endif

        case BT_SINK_SRV_MUSIC_DEVICE_PSE_HD: {
            pse_hd = (audio_src_srv_handle_t *) param;
            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if ((ctx->sink_dev[i].handle) &&
                    (ctx->sink_dev[i].handle == pse_hd)) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_LOCAL: {
            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                //bt_sink_srv_report("[sink][music]get local %d %x", i, &(ctx->sink_dev[i]));
                if (ctx->sink_dev[i].mp3) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_SP: {
            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if ((ctx->sink_dev[i].conn_bit & BT_SINK_SRV_MUSIC_A2DP_CONN_BIT) &&
                    (ctx->sink_dev[i].conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }
            break;
        }

        default:
            break;
    }

    if (!g_sink_music_log_disable) {
        bt_sink_srv_music_report("[sink][music]get_dev-dev: 0x%x, type: %d, param: 0x%x\n",
                           dev, type, param);
    }

    return dev;
}


void bt_sink_srv_music_reset_device(bt_sink_srv_music_device_t *dev)
{
    bt_sink_srv_music_context_t *ctx = NULL;

    bt_sink_srv_assert(dev);
    bt_sink_srv_memset(dev, 0x00, sizeof(bt_sink_srv_music_device_t));
    dev->role = BT_A2DP_INVALID_ROLE;
#ifdef __BT_AWS_SUPPORT__    
    dev->aws_role = BT_AWS_ROLE_NONE;
#endif

    /* Update device */
    ctx = bt_sink_srv_music_get_context();

    ctx->latest_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_USED, NULL);
    bt_sink_srv_music_set_focus_device(ctx->latest_dev);
}


void bt_sink_srv_music_set_focus_device(bt_sink_srv_music_device_t *dev)
{
    bt_sink_srv_music_context_t *ctx = NULL;

    ctx = bt_sink_srv_music_get_context();

    ctx->focus_dev = dev;
}


void bt_sink_srv_music_update_run_device(bt_sink_srv_music_device_t *dev)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();

    bt_sink_srv_report("[sink][music]update_run_device--run: 0x%08x", dev);
    ctx->run_dev = dev;

    if (dev) {
        bt_sink_srv_music_set_focus_device(dev);
    }
}


void bt_sink_srv_music_drv_play(void *param)
{
    bt_sink_srv_music_device_t *dev = (bt_sink_srv_music_device_t *)param;
    if (AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP == dev->handle->type) {
        bt_sink_srv_a2dp_drv_play(param);
    } 
#ifdef __BT_AWS_SUPPORT__
    else if (AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP == dev->handle->type) {
        bt_sink_srv_aws_a2dp_drv_play(param);
    } 
#endif
    else {
        bt_sink_srv_report("[sink][music]drv_play(error)--type: %d", dev->handle->type);
    }
}


void bt_sink_srv_music_drv_stop(void *param)
{
    bt_sink_srv_music_device_t *dev = (bt_sink_srv_music_device_t *)param;
    if (AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP == dev->handle->type) {
        bt_sink_srv_a2dp_drv_stop(param);
    }
#ifdef __BT_AWS_SUPPORT__
    else if (AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP == dev->handle->type) {
        bt_sink_srv_aws_a2dp_drv_stop(param);
    } 
#endif
    else {
        bt_sink_srv_report("[sink][music]drv_stop(error)--type: %d", dev->handle->type);
    }
}


void bt_sink_srv_music_fill_audio_src_callback(audio_src_srv_handle_t *handle)
{
    bt_sink_srv_assert(handle);
    handle->play = bt_sink_srv_music_play_handle;
    handle->stop = bt_sink_srv_music_stop_handle;
    handle->suspend = bt_sink_srv_music_suspend_handle;
    handle->reject = bt_sink_srv_music_reject_handle;
    handle->exception_handle = bt_sink_srv_music_exception_handle;
}


int32_t bt_sink_srv_music_start_gpt_timer(hal_gpt_callback_t callback, void *user_data, uint32_t duration)
{
    hal_gpt_port_t gpt_port = HAL_GPT_MAX_PORT;
    hal_gpt_status_t gpt_ret = HAL_GPT_STATUS_ERROR_PORT_USED;

    if (hal_gpt_init(HAL_GPT_1) == HAL_GPT_STATUS_OK) {
        gpt_port = HAL_GPT_1;
    } else if (hal_gpt_init(HAL_GPT_2) == HAL_GPT_STATUS_OK) {
        gpt_port = HAL_GPT_2;
    } else {
        ;
    }

    if (gpt_port != HAL_GPT_MAX_PORT) {
        hal_gpt_register_callback(gpt_port,
            callback, user_data);
        //sink_loc_play_nclk_intra += duration;
        gpt_ret = hal_gpt_start_timer_us(gpt_port, duration - 2000, HAL_GPT_TIMER_TYPE_ONE_SHOT);
    } else {
        bt_sink_srv_report("[sink][music]start_gpt_timer(error)--dur: %d", duration);
    }

    bt_sink_srv_report("[sink][music]start_gpt_timer--ret: %d", gpt_ret);

    return gpt_port;
}


void BT_A2DP_MAKE_SBC_CODEC(bt_a2dp_codec_capability_t *codec,
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


void BT_A2DP_MAKE_AAC_CODEC(bt_a2dp_codec_capability_t *codec,
                            bt_a2dp_role_t role, bool vbr, uint8_t object_type,
                            uint8_t channels, uint16_t sample_rate,
                            uint32_t bit_rate)
{
    do {
        codec->type = BT_A2DP_CODEC_AAC;
        codec->sep_type = role;
        codec->length = sizeof(bt_a2dp_aac_codec_t);
        codec->codec.aac.object_type = object_type;
        codec->codec.aac.freq_h = ((sample_rate >> 4) & 0xFF);
        codec->codec.aac.reserved = 0x03;
        codec->codec.aac.channels = channels;
        codec->codec.aac.freq_l = (sample_rate & 0x0F);
        codec->codec.aac.br_h = ((bit_rate >> 16) & 0x7F);
        codec->codec.aac.vbr = vbr;
        codec->codec.aac.br_m = ((bit_rate >> 8) & 0xFF);
        codec->codec.aac.br_l = (bit_rate & 0xFF);
    } while (0);
}


void BT_A2DP_CONVERT_SBC_CODEC(bt_codec_capability_t *dst_codec,
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
    bt_sink_srv_report("[sink][am][med]CONVERT_SBC--min_pool: %d, max_pool: %d, b_len: %d, sub_num: %d, all_met: %d, samp_rate: %d, ch_m: %d",
                       dst_codec->codec.sbc.min_bit_pool, dst_codec->codec.sbc.max_bit_pool,
                       dst_codec->codec.sbc.block_length, dst_codec->codec.sbc.subband_num,
                       dst_codec->codec.sbc.alloc_method, dst_codec->codec.sbc.sample_rate,
                       dst_codec->codec.sbc.channel_mode);
}


void BT_A2DP_CONVERT_AAC_CODEC(bt_codec_capability_t *dst_codec,
                               bt_a2dp_codec_capability_t *src_codec)
{
    dst_codec->type = BT_A2DP_CODEC_AAC;
    dst_codec->codec.aac.vbr = src_codec->codec.aac.vbr;
    dst_codec->codec.aac.object_type = src_codec->codec.aac.object_type;
    dst_codec->codec.aac.channels = src_codec->codec.aac.channels;
    dst_codec->codec.aac.sample_rate = (src_codec->codec.aac.freq_h << 4) | (src_codec->codec.aac.freq_l);
    dst_codec->codec.aac.bit_rate = (src_codec->codec.aac.br_h << 16) | (src_codec->codec.aac.br_m << 8) | (src_codec->codec.aac.br_l);
    bt_sink_srv_report("[sink][am][med]CONVERT_AAC--vbr: %d, object_type: %d, channels: %d, sample_rate: %d, bit_rate: %d",
                       dst_codec->codec.aac.vbr, dst_codec->codec.aac.object_type,
                       dst_codec->codec.aac.channels, dst_codec->codec.aac.sample_rate,
                       dst_codec->codec.aac.bit_rate);
}


void bt_sink_srv_music_disable_log(void)
{
    g_sink_music_log_disable = true;
}


void bt_sink_srv_music_enable_log(void)
{
    g_sink_music_log_disable = false;
}


uint64_t bt_sink_srv_music_convert_btaddr_to_devid(bt_bd_addr_t *bd_addr)
{
    uint64_t dev_id = 0;
    uint32_t hdev = 0, ldev = 0;
    int32_t i = 0;
    uint8_t addr[16] = {0};

    bt_sink_srv_assert(bd_addr);
    bt_sink_srv_memcpy(addr, bd_addr, sizeof(bt_bd_addr_t));
    for (i = 0; i < BT_BD_ADDR_LEN; ++i) {
        dev_id = ((dev_id << 8) | (addr[i]));
    }

    hdev = (dev_id >> 32 & 0xffffffff);
    ldev = (dev_id & 0xffffffff);

    bt_sink_srv_report("[sink][music]convert_btaddr_to_devid--hdev: 0x%x, ldev: 0x%x", hdev, ldev);

    return dev_id;
}


#ifdef __BT_AWS_SUPPORT__
int32_t bt_sink_srv_music_get_bt_clk_offset(uint32_t handle, bt_sink_srv_music_callback_t callback, void *user_data)
{
    int32_t ret = 0;
    bt_sink_srv_music_context_t *ctx = NULL;

    ctx = bt_sink_srv_music_get_context();
    if ((ctx->callback) || !(callback)) {
        return BT_SINK_SRV_MUSIC_ERR_FAIL_7TH;
    }

    ret = bt_aws_fetch_bt_time_offset(handle);
    if (BT_STATUS_SUCCESS == ret) {
        ctx->callback = callback;
        ctx->user_data = user_data;
    }
    return ret;
}


void bt_sink_srv_music_disable_transfer_packet(void)
{
    bt_sink_srv_music_context_t *ctx = NULL;

    ctx = bt_sink_srv_music_get_context();
    ctx->trans = false;
    ctx->first_trans = false;
    bt_sink_srv_report("[sink][music]disable transfer aws packet");
}


void bt_sink_srv_music_enable_transfer_packet(void)
{
    bt_sink_srv_music_context_t *ctx = NULL;

    ctx = bt_sink_srv_music_get_context();
    ctx->trans = true;
    ctx->first_trans = true;
    bt_sink_srv_report("[sink][music]enable transfer aws packet");
}


bool bt_sink_srv_music_duration_valid(bt_aws_module_id_t module_id, uint32_t duration)
{
    uint32_t dur_max = BT_SINK_SRV_MUSIC_DUR_MAX;
    uint32_t dur_min = BT_SINK_SRV_MUSIC_DUR_MIN;

    if (module_id == BT_SINK_SRV_AWS_MODULE_ID_MP3) {
        dur_max = BT_SINK_SRV_MUSIC_MP3_DUR_MAX;
        dur_min = BT_SINK_SRV_MUSIC_MP3_DUR_MIN;
    }

    if (duration >= dur_min && duration <= dur_max) {
        return true;
    }

    return false;
}


void *bt_sink_srv_music_find_suitable_packet(bt_sink_srv_music_device_t *dev, uint32_t *time_dur, uint8_t type)
{
    bt_hci_packet_t *pkt = NULL;
    bt_sink_srv_music_context_t *ctx = NULL;
    uint32_t time_stamp= 0;
    sink_srv_clock_mapping_t base;
    uint32_t duration = 0;
    bt_aws_clock_t cur_bt_clk = {0};

    ctx = bt_sink_srv_music_get_context();

    /* Local bt time */
    if (type == 0) {
        base.seq_idx = ctx->local_info.pkt_info.sn;
        base.time_stamp = ctx->local_info.pkt_info.ts;
        base.piconet_clk.nclk = ctx->local_info.clock.nclk;
        base.piconet_clk.nclk_intra = ctx->local_info.clock.nclk_intra;
    } else if (type == 1) {
        base.seq_idx = ctx->piconet_info.pkt_info.sn;
        base.time_stamp = ctx->piconet_info.pkt_info.ts;
        base.piconet_clk.nclk = ctx->piconet_info.clock.nclk;
        base.piconet_clk.nclk_intra = ctx->piconet_info.clock.nclk_intra;
    }

    bt_sink_srv_report("[sink][music]find(s)--dev: 0x%08x, gap_role: %d, offset: 0x%08x, intra: 0x%08x, type: %d",
        dev, dev->gap_role, ctx->clk_offset.offset, ctx->clk_offset.offset_intra, type);

    while (1) {
        /* Get first item */
        pkt = (bt_hci_packet_t *)bt_sink_srv_media_mgr_get_first_item();
        if (!pkt) {
            bt_sink_srv_report("[sink][music]find(empty)");
            break;
        }

        time_stamp = sink_srv_util_get_a2dp_time_stamp(pkt);
        ctx->med_play_info.sequence_idx = sink_srv_util_get_a2dp_sn(pkt);
        if (type == 0) {
            bt_aws_get_bt_local_time(&cur_bt_clk);
        } else if (type == 1) {
            bt_aws_get_curr_bt_time(dev->gap_role, ctx->clk_offset, &cur_bt_clk);
        }

        duration = sink_srv_get_play_duration(&(ctx->med_play_info), &base, &cur_bt_clk, time_stamp);

        if (bt_sink_srv_music_duration_valid(BT_SINK_SRV_AWS_MODULE_ID_A2DP, duration)) {
            *time_dur = duration;
            bt_sink_srv_report("[sink][music]find(valid)--dur: 0x%08x, nclk: 0x%08x, intra: 0x%08x", duration, cur_bt_clk.nclk, cur_bt_clk.nclk_intra);
            break;
        } else {
            bt_sink_srv_report("[sink][music]find(invalid)--dur: 0x%08x, nclk: 0x%08x, intra: 0x%08x", duration, cur_bt_clk.nclk, cur_bt_clk.nclk_intra);
        }

        /* Pop first item */
        bt_sink_srv_media_mgr_pop_first_item();
    }

    return pkt;
}


void bt_sink_srv_music_update_aws_sync_info(bt_sink_srv_music_device_t *dev, bt_hci_packet_t *packet, uint8_t type)
{
    bt_sink_srv_music_context_t *ctx = NULL;

    ctx = bt_sink_srv_music_get_context();

    if (type == 0) {
        /* Update local sync info */
        bt_aws_get_bt_local_time(&(ctx->local_info.clock));
        ctx->local_info.pkt_info.sn = sink_srv_util_get_a2dp_sn(packet);
        ctx->local_info.pkt_info.ts = sink_srv_util_get_a2dp_time_stamp(packet);
    } else if (type == 1) {
        /* Update bt sync info */
        bt_aws_get_curr_bt_time(dev->gap_role, ctx->clk_offset, &ctx->piconet_info.clock);
        ctx->piconet_info.pkt_info.sn = sink_srv_util_get_a2dp_sn(packet);
        ctx->piconet_info.pkt_info.ts = sink_srv_util_get_a2dp_time_stamp(packet);
    }
}
#endif /* __BT_AWS_SUPPORT__*/
