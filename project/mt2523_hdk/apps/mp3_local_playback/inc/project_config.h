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

/* max supported connection number */
#define BT_CONNECTION_MAX   10

#define BT_MAX_LINK_NUM 2

/* max timer count */
#define BT_TIMER_NUM 10

#define BT_TX_BUF_SIZE 256*100
#define BT_RX_BUF_SIZE 2048*10

#define BT_TIMER_BUF_SIZE (BT_TIMER_NUM * BT_CONTROL_BLOCK_SIZE_OF_TIMER)
#define BT_LE_CONNECTION_BUF_SIZE (BT_CONNECTION_MAX* BT_CONTROL_BLOCK_SIZE_OF_LE_CONNECTION)
#define TOTAL_L2CAP_CHANNEL_NUM (6 + 5) /**<[IMPORTANT!]total num = N1 + N2 + ..., Nx is the really used channel num for link-x, each link may different*/
#define BT_L2CAP_CHANNEL_BUF_SIZE (TOTAL_L2CAP_CHANNEL_NUM * BT_CONTROL_BLOCK_SIZE_OF_L2CAP)

#define BT_RFCOMM_TOTAL_LINK_NUM 3 /**<[IMPORTANT!]total num = N, N is the acl link num that rfcomm support*/
#define BT_RFCOMM_LINK_BUF_SIZE (BT_RFCOMM_TOTAL_LINK_NUM * BT_CONTROL_BLOCK_SIZE_OF_RFCOMM)

#define BT_HFP_TOTAL_LINK_NUM BT_MAX_LINK_NUM
#define BT_HFP_LINK_BUF_SIZE (BT_HFP_TOTAL_LINK_NUM * BT_CONTROL_BLOCK_SIZE_OF_HFP)

#define BT_AVRCP_TOTAL_LINK_NUM 2
#define BT_AVRCP_LINK_BUF_SIZE (BT_AVRCP_TOTAL_LINK_NUM * BT_CONTROL_BLOCK_SIZE_OF_AVRCP)

#define BT_A2DP_SEP_TOTAL_NUM 4
#define BT_A2DP_SEP_BUF_SIZE (BT_A2DP_SEP_TOTAL_NUM * BT_CONTROL_BLOCK_SIZE_OF_A2DP_SEP)

#define BT_A2DP_TOTAL_LINK_NUM 2
#define BT_A2DP_LINK_BUF_SIZE (BT_A2DP_TOTAL_LINK_NUM * BT_CONTROL_BLOCK_SIZE_OF_A2DP)

#define BT_SPP_TOTAL_CONNECTION_NUM  (3 + 2) /**<[IMPORTANT!]total num = N1 + N2 + ..., Nx is the really used connection num for link-x, each link may different*/
#define BT_SPP_CONNECTION_BUF_SIZE (BT_SPP_TOTAL_CONNECTION_NUM * BT_CONTROL_BLOCK_SIZE_OF_SPP)

#define BT_PBAPC_TOTAL_CONNECTION_NUM  2 /**<[IMPORTANT!]total num = N1 + N2 + ..., Nx is the really used connection num for link-x, each link may different*/
#define BT_PBAPC_CONNECTION_BUF_SIZE (BT_PBAPC_TOTAL_CONNECTION_NUM * BT_CONTROL_BLOCK_SIZE_OF_PBAPC)
