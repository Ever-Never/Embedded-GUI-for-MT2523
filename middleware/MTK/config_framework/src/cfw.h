/* Copyright Statement:
 *
 * (C) 2005-2017 MediaTek Inc. All rights reserved.
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

#ifndef __CFW_H__
#define __CFW_H__
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define cfw_type_int8_t           1
#define cfw_type_uint8_t          2
#define cfw_type_int16_t          3
#define cfw_type_uint16_t         4
#define cfw_type_int32_t          5
#define cfw_type_uint32_t         6
#define cfw_type_array_int8_t     7
#define cfw_type_array_uint8_t    8
#define cfw_type_array_char       9

#undef CFW_ITEM
#undef CFW_ARRAY_ITEM
#define CFW_ITEM(key, name, type, value) CFW_INDEX_##key,
#define CFW_ARRAY_ITEM(key, name, type, size, args...) CFW_INDEX_##key,
typedef enum {
#include "cfw_item.h"
    CFW_INDEX_DUMMY,
    CFW_INDEX_END
} cfw_item_index_t;

typedef struct {
    uint16_t key;
    uint16_t offset;
    uint16_t type;
    uint16_t size;
} cfw_access_table_item_t;

typedef struct {
    uint16_t key;
    uint16_t type;
    uint16_t size;
    uint8_t* address;
    char var_name[32];
} cfw_data_info_t;

int32_t cfw_write_data(uint16_t key, const uint8_t* buff, uint32_t* size);
int32_t cfw_get_data_info_by_key(cfw_data_info_t* info);
int32_t cfw_get_data_info_by_index(cfw_data_info_t* info, uint32_t index);
int32_t cfw_flush(void);
uint32_t cfw_get_max_item_count(void);
#ifdef __CFW_CONFIG_MODE__
extern const cfw_access_table_item_t access_table[];
extern const char var_name[CFW_INDEX_END][32];
#endif
#endif
