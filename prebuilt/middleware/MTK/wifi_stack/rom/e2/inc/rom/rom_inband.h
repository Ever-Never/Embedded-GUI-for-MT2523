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


#ifndef __ROM_INBAND_H__
#define __ROM_INBAND_H__

#define MAX_WEP_KEY_SIZE_CM4 32
typedef struct _wep_key_cm4 {
    uint8_t ucdefault_key_id;
    uint8_t auckey_0[MAX_WEP_KEY_SIZE_CM4];
    uint8_t uckey_0_len;
    uint8_t auckey_1[MAX_WEP_KEY_SIZE_CM4];
    uint8_t uckey_1_len;
    uint8_t auckey_2[MAX_WEP_KEY_SIZE_CM4];
    uint8_t uckey_2_len;
    uint8_t auckey_3[MAX_WEP_KEY_SIZE_CM4];
    uint8_t uckey_3_len;
} wep_key_cm4_t;

typedef enum _extra_ie_type {
    WIFI_EXTRA_WPS_BEACON_IE = 0,
    WIFI_EXTRA_WPS_PROBE_REQ_IE = 1,
    WIFI_EXTRA_WPS_PROBE_RSP_IE = 2,
    WIFI_EXTRA_WPS_ASSOC_REQ_IE = 3,
    WIFI_EXTRA_WPS_ASSOC_RSP_IE = 4,
#ifdef MTK_WIFI_DIRECT_ENABLE
    WIFI_EXTRA_P2P_BEACON_IE = 5,
    WIFI_EXTRA_P2P_PROBE_REQ_IE = 6,
    WIFI_EXTRA_P2P_PROBE_RSP_IE = 7,
    WIFI_EXTRA_P2P_ASSOC_REQ_IE = 8,
    WIFI_EXTRA_P2P_ASSOC_RSP_IE = 9,
#endif /*MTK_WIFI_DIRECT_ENABLE*/
/* CONFIG_FREE_SUPPORT */
	WIFI_EXTRA_CF_PROBE_REQ_IE = 10,
	WIFI_EXTRA_CF_PROBE_RSP_IE = 11, 

} extra_ie_type_t;

typedef struct _extra_ie {
    uint8_t port;
    uint8_t type; /* extra_ie_type */
    uint8_t rsv[2];
    uint32_t extra_ie_len;
#ifndef __CC_ARM
    uint8_t extra_ie[0];
#else
    uint8_t extra_ie[];
#endif
} extra_ie_t;

#endif
