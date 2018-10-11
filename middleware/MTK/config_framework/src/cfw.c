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

#include "cfw_define.h"
#include "cfw.h"
#include "hal_flash.h"
#include "memory_map.h"
#include "syslog.h"

#ifdef __CFW_CONFIG_MODE__
static uint32_t load_address;
static uint8_t  flash_data[4096];
static int32_t cfw_write_flash_int(uint32_t dest, const uint8_t* buff, uint32_t size);
static uint32_t cfw_get_index_by_key(uint16_t key);

int32_t cfw_write_data(uint16_t key, const uint8_t* buff, uint32_t* size)
{
    cfw_data_info_t info;
    info.key = key;
    if (cfw_get_data_info_by_key(&info) != 0) {
        LOG_I(common, "[CFW]cfw_write_data return -1");
        return -1;
    }
    if (info.size < *size) {
        *size = info.size;
    }
    return cfw_write_flash_int((uint32_t)(info.address), buff, *size);
}

static int32_t cfw_write_flash_int(uint32_t dest, const uint8_t* buff, uint32_t size)
{
    int32_t ret;
    uint32_t write = size;
    if ((load_address != 0) && (load_address != (dest & 0xFFFFF000)))
    {
        LOG_I(common, "[CFW]cfw_write_flash_int flush old data");
        ret = cfw_flush();
        if (0 != ret) {
            LOG_I(common, "[CFW]cfw_write_flash_int error");
            return ret;
        }
    }
    if (load_address == 0) {
        //load data.
        LOG_I(common, "[CFW]cfw_write_flash_int load flash data");
        load_address = dest & 0xFFFFF000;
        memcpy(flash_data, (uint8_t*)load_address, sizeof(flash_data));
    }
    if (load_address != ((dest + size) & 0xFFFFF000))
    {
        write = load_address + 0x1000 - dest;
        LOG_I(common, "[CFW]cfw_write_flash_int write part of data %d", write);
        memcpy(flash_data + dest - load_address, buff, write);
        return cfw_write_flash_int(load_address + 0x1000, buff + write, size - write);
    }
    LOG_I(common, "[CFW]cfw_write_flash_int write data %d", write);
    memcpy(flash_data + dest - load_address, buff, write);
    return 0;
}


int32_t cfw_get_data_info_by_key(cfw_data_info_t* info)
{
    uint32_t index;
    if (info->key == 0)
    {
        return -1;
    }
    index = cfw_get_index_by_key(info->key);
    return cfw_get_data_info_by_index(info, index);
}
int32_t cfw_get_data_info_by_index(cfw_data_info_t* info, uint32_t index)
{
    if (index >= CFW_INDEX_DUMMY) {
        return -1;
    }
    info->key = access_table[index].key;
    info->type = access_table[index].type;
    info->size = access_table[index].size;
    info->address = ((uint8_t*)cfw_data_ptr) + access_table[index].offset;
    memcpy(info->var_name, var_name[index], 32);
    return 0;
}

int32_t cfw_flush(void)
{
    hal_flash_status_t ret;
    if (load_address != 0) {
        ret = hal_flash_erase(load_address - BL_BASE, HAL_FLASH_BLOCK_4K);
        if (HAL_FLASH_STATUS_OK != ret) {
            LOG_I(common, "[CFW] erase %x error %d", load_address - BL_BASE, ret);
            return -3;
        }
        ret = hal_flash_write(load_address - BL_BASE, flash_data, sizeof(flash_data));
        if (HAL_FLASH_STATUS_OK != ret) {
            LOG_I(common, "[CFW] flash write %x, size %d error %d", flash_data, sizeof(flash_data), ret);
            return -4;
        }
    }
    load_address = 0;
    return 0;
}

uint32_t cfw_get_max_item_count(void)
{
    return (uint32_t)CFW_INDEX_DUMMY;
}

static uint32_t cfw_get_index_by_key(uint16_t key)
{
    int i;
    for (i = 0; i < CFW_INDEX_DUMMY; i++) {
        if (access_table[i].key == key) {
            break;
        }
    }
    return i;
}
#endif
