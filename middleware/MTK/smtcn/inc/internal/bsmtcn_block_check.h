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
 
#ifndef BSMTCN_BLOCK_CHECK_H
#define BSMTCN_BLOCK_CHECK_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <stdbool.h>


#pragma pack(1)
#if 0
 typedef struct  bsmtcn_payload_struct  
 {
	uint16_t total_length;
	uint8_t version;
    uint16_t crc16;
	struct  { 
	uint32_t reserved:8;
	uint32_t ssid_len:6;
	uint32_t password_len:8;
	uint32_t extend_data_len:10;
#if defined(__ICCARM__)
    } __packed mix_len;
#else
    } __attribute__((__packed__)) mix_len;
#endif

#if defined(__ICCARM__)
} __packed  bsmtcn_payload_struct_t;
#else
} __attribute__((__packed__)) bsmtcn_payload_struct_t;
#endif

#endif
 typedef struct  bsmtcn_payload_struct  
 {
	uint16_t total_length;
	uint8_t version;
    uint16_t crc16;
	struct  { 
	uint32_t reserved:8;
	uint32_t ssid_len:6;
	uint32_t password_len:8;
	uint32_t extend_data_len:10;

    }  mix_len;


} bsmtcn_payload_struct_t;

#pragma pack()


void bsmtcn_block_get_payload(uint16_t *packet_table);

void bsmtcn_block_unscramble(uint16_t *packet_table);

bool bsmtcn_block_crc_check(uint16_t *packet_table);

int32_t bsmtcn_packet_decoder(uint8_t *addr, uint16_t len, uint8_t from_ds_flag);

bool bsmtcn_payload_crc_check(uint8_t *payload, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* BSMTCN_BLOCK_CHECK_H */

