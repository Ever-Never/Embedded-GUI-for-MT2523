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


bt_sink_srv_music_context_t g_bt_sink_srv_cntx;

extern SinkProfileA2dp sink_a2dp[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];

extern SinkProfileAvrcp sink_avrcp[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];

#ifdef __BT_AWS_SUPPORT__
extern SinkProfileAws sink_aws[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];
#endif

void bt_sink_srv_music_init(void)
{
    int32_t i = 0;

    memset(&g_bt_sink_srv_cntx, 0x00, sizeof(bt_sink_srv_music_context_t));

    for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
        g_bt_sink_srv_cntx.sink_dev[i].a2dp = &sink_a2dp[i];
        g_bt_sink_srv_cntx.sink_dev[i].avrcp = &sink_avrcp[i];
        #ifdef __BT_AWS_SUPPORT__
        g_bt_sink_srv_cntx.sink_dev[i].aws = &sink_aws[i];
        #endif
        /* init invalid a2dp & avrcp & aws handle */
        g_bt_sink_srv_cntx.sink_dev[i].a2dp_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
        g_bt_sink_srv_cntx.sink_dev[i].avrcp_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
        g_bt_sink_srv_cntx.sink_dev[i].aws_hd = BT_SINK_SRV_MUSIC_INVALID_HD;

        g_bt_sink_srv_cntx.sink_dev[i].role = BT_A2DP_INVALID_ROLE;
        #ifdef __BT_AWS_SUPPORT__
        g_bt_sink_srv_cntx.sink_dev[i].aws_role = BT_AWS_ROLE_NONE;
        #endif
    }
    g_bt_sink_srv_cntx.partymode = BT_SINK_SRV_PARTYMODE_INTERRUPT;
    g_bt_sink_srv_cntx.a2dp_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
    g_bt_sink_srv_cntx.aud_id = BT_SINK_SRV_INVALID_AID;
    g_bt_sink_srv_cntx.aws_aid = BT_SINK_SRV_INVALID_AID;
    g_bt_sink_srv_cntx.gpt_port = HAL_GPT_MAX_PORT;
    g_bt_sink_srv_cntx.vol_lev = AUD_VOL_OUT_LEVEL4;
    bt_sink_srv_report("[sink][music]init-\r\n");
}


bt_sink_srv_music_context_t *bt_sink_srv_music_get_context(void)
{
    return &g_bt_sink_srv_cntx;
}


bt_sink_srv_music_device_t *bt_sink_srv_music_get_device(bt_sink_srv_music_device_type_t type, const void *param)
{
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;
    bt_bd_addr_t *dev_addr = NULL;
    int32_t i = 0;
    uint32_t *p_hd = NULL;
    bool aws_exist = false;

    cntx = bt_sink_srv_music_get_context();

    switch (type) {
        case BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD: {
            p_hd = (uint32_t *)param;

            for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
                if (cntx->sink_dev[i].a2dp_hd != BT_SINK_SRV_MUSIC_INVALID_HD
                        && cntx->sink_dev[i].a2dp_hd == *p_hd) {
                    dev = &(cntx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_AVRCP_HD: {
            p_hd = (uint32_t *)param;

            for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
                if (cntx->sink_dev[i].avrcp_hd != BT_SINK_SRV_MUSIC_INVALID_HD
                        && cntx->sink_dev[i].avrcp_hd == *p_hd) {
                    dev = &(cntx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_ADDR: {
            dev_addr = (bt_bd_addr_t *)param;

            for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
                if ((cntx->sink_dev[i].a2dp_hd != BT_SINK_SRV_MUSIC_INVALID_HD ||
                        cntx->sink_dev[i].avrcp_hd != BT_SINK_SRV_MUSIC_INVALID_HD ||
                        cntx->sink_dev[i].aws_hd != BT_SINK_SRV_MUSIC_INVALID_HD) &&
                        (memcmp(dev_addr, &(cntx->sink_dev[i].dev_addr), sizeof(bt_bd_addr_t)) == 0)) {
                    dev = &(cntx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_UNUSED: {
            for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
                if (cntx->sink_dev[i].a2dp_hd == BT_SINK_SRV_MUSIC_INVALID_HD &&
                    cntx->sink_dev[i].aws_hd == BT_SINK_SRV_MUSIC_INVALID_HD) {
                    dev = &(cntx->sink_dev[i]);
                    cntx->latest_dev = dev;
                    break;
                }
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_USED: {
            for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
                if (cntx->sink_dev[i].a2dp_hd != BT_SINK_SRV_MUSIC_INVALID_HD) {
                    dev = &(cntx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_LATEST: {
            dev = cntx->latest_dev;
            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_INTERRUPT: {
            for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
                if (cntx->sink_dev[i].flag & BT_SINK_SRV_MUSIC_FLAG_INTERRUPT) {
                    dev = &(cntx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_FOCUS: {
            for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
                if (cntx->sink_dev[i].conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT) {
                    aws_exist = true;
                    break;
                }
            }

            /* find SP dev information */
            if (aws_exist) {
                for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
                    if ((cntx->sink_dev[i].conn_bit & BT_SINK_SRV_MUSIC_A2DP_CONN_BIT) &&
                        (cntx->sink_dev[i].conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
                        dev = &(cntx->sink_dev[i]);
                        break;
                    }
                }
            } else {
                /* if there are no streaming dev, the latest conneced dev will be
                the focus dev */
                if (cntx->focus_dev) {
                    dev = cntx->focus_dev;
                } else {
                    dev = cntx->latest_dev;
                }
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_AWS_HD: {
            p_hd = (uint32_t *)param;

            for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
                if (cntx->sink_dev[i].aws_hd != BT_SINK_SRV_MUSIC_INVALID_HD
                        && cntx->sink_dev[i].aws_hd == *p_hd) {
                    dev = &(cntx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_AWS: {
            for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
                if ((cntx->sink_dev[i].aws_hd != BT_SINK_SRV_MUSIC_INVALID_HD) &&
                    (cntx->sink_dev[i].conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT)) {
                    dev = &(cntx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        default:
            break;
    }

    if ((cntx->state != BT_SINK_SRV_MUSIC_PLAY) &&
        (cntx->state != BT_SINK_SRV_MUSIC_AWS_PLAY)) {
        bt_sink_srv_music_report("[sink][music]get_dev-dev: 0x%x, type: %d, param: 0x%x\n",
                           dev, type, param);
    } else if ((cntx->state == BT_SINK_SRV_MUSIC_PLAY && cntx->log_cnt < BT_SINK_SRV_MUSIC_MAX_LOG_COUNT) ||
               (cntx->state == BT_SINK_SRV_MUSIC_AWS_PLAY && cntx->log_cnt < BT_SINK_SRV_MUSIC_MAX_LOG_COUNT)) {
        cntx->log_cnt += 1;
        bt_sink_srv_music_report("[sink][music]get_dev-dev: 0x%x, type: %d, param: 0x%x\n",
                           dev, type, param);
    }

    return dev;
}


void bt_sink_srv_music_reset_device(bt_sink_srv_music_device_t *dev)
{
    bt_sink_srv_music_context_t *cntx = NULL;

    cntx = bt_sink_srv_music_get_context();

    dev->a2dp_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
    dev->avrcp_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
    dev->aws_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
    dev->conn_bit = 0x00;
    dev->flag = 0x00;

    dev->role = BT_A2DP_INVALID_ROLE;
    #ifdef __BT_AWS_SUPPORT__
    dev->aws_role = BT_AWS_ROLE_NONE;
    #endif /* __BT_AWS_SUPPORT__ */
    /* update latest dev */
    cntx->latest_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_USED, NULL);

    /* update focus dev */
    bt_sink_srv_music_set_focus_device(cntx->latest_dev);
}


void bt_sink_srv_music_set_focus_device(bt_sink_srv_music_device_t *dev)
{
    bt_sink_srv_music_context_t *cntx = NULL;

    cntx = bt_sink_srv_music_get_context();

    cntx->focus_dev = dev;
}

