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

#include "bt_sink_srv_custom_db.h"
#include "bt_sink_srv_utils.h"
#ifdef WIN32
#include "btosapi.h"
#endif
#ifdef __BT_SINK_SRV_NVDM_SUPPORT__
#include "nvdm.h"
#endif /* __BT_SINK_SRV_NVDM_SUPPORT__ */

#ifdef WIN32
bool bt_sink_srv_db_read_files(uint8_t info_type, uint8_t *buffer, uint32_t size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    OS_FS_HANDLE file_handle;
    int32_t file_size;
    int32_t temp_size = 0;
    uint8_t name[FS_MAX_PATH_LEN] = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* check path */
    if (OS_DirExist((const uint8_t *)BT_SINK_SRV_DB_PATH) == false) {
        OS_MakeDir((const uint8_t *)BT_SINK_SRV_DB_PATH);
    }

    OS_SetCurrentDir((const uint8_t *)BT_SINK_SRV_DB_PATH);

    if (info_type == BT_SINK_SRV_CUSTOM_DB_DEV_INFO) {
        bt_sink_srv_strcat(name, BT_SINK_SRV_DB_FILE);
    } else if (info_type == BT_SINK_SRV_CUSTOM_DB_SCAN_INFO) {
        bt_sink_srv_strcat(name, BT_SINK_SRV_SCAN_DB_FILE);
    } else if (info_type == BT_SINK_SRV_CUSTOM_DB_SDAP_INFO) {
        bt_sink_srv_strcat(name, BT_SINK_SRV_SDAP_DEVDB_FILE);
    } else {
        bt_sink_srv_assert(0);
    }

    file_handle = OS_FileOpen((const uint8_t *)name, OS_FS_READ_ONLY);

    if (file_handle == NULL) {
        bt_sink_srv_report("[ERR] db %s not exist", name);
    } else {
        file_size = OS_FileSeek(file_handle, 0, OS_FS_SEEK_END);
        if (file_size != (int32_t)size) {
            OS_FileClose(file_handle);
            bt_sink_srv_report("db %s file size %d incorrect : expected %u", name, file_size, size);
        } else {
            OS_FileSeek(file_handle, 0, OS_FS_SEEK_BEGIN);
            OS_FileRead(file_handle, (uint8_t *)buffer, size);
            OS_FileClose(file_handle);
            return true;
        }
    }
    file_handle = OS_FileOpen((const uint8_t *)name, OS_FS_READ_WRITE | OS_FS_CREATE);
    bt_sink_srv_memset(buffer, 0, size);
    OS_FileWrite(file_handle, (uint8_t *)buffer, size);
    OS_FileClose(file_handle);
    return false;
}

static bool bt_sink_srv_db_wirte_files(uint8_t info_type, uint8_t *buffer, uint32_t size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    OS_FS_HANDLE file_handle;
    uint8_t name[FS_MAX_PATH_LEN] = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* check path */
    if (OS_DirExist((const uint8_t *)BT_SINK_SRV_DB_PATH) == false) {
        OS_MakeDir((const uint8_t *)BT_SINK_SRV_DB_PATH);
    }

    OS_SetCurrentDir((const uint8_t *)BT_SINK_SRV_DB_PATH);

    if (info_type == BT_SINK_SRV_CUSTOM_DB_DEV_INFO) {
        bt_sink_srv_strcat(name, BT_SINK_SRV_DB_FILE);
    } else if (info_type == BT_SINK_SRV_CUSTOM_DB_SCAN_INFO) {
        bt_sink_srv_strcat(name, BT_SINK_SRV_SCAN_DB_FILE);
    } else if (info_type == BT_SINK_SRV_CUSTOM_DB_SDAP_INFO) {
        bt_sink_srv_strcat(name, BT_SINK_SRV_SDAP_DEVDB_FILE);
    } else {
        bt_sink_srv_assert(0);
    }

    file_handle = OS_FileOpen((const uint8_t *)name, OS_FS_READ_WRITE | OS_FS_CREATE_ALWAYS);
    OS_FileWrite(file_handle, buffer, size);
    OS_FileClose(file_handle);
    return true;
}
#endif /* WIN32 */

#ifdef __BT_SINK_SRV_NVDM_SUPPORT__
bool bt_sink_srv_db_rw_nvdm(uint8_t info_type, uint8_t *buffer, uint32_t size, bool read)
{
    const char *group_name = NULL;
    const char *item_name = NULL;
    nvdm_status_t result = NVDM_STATUS_ERROR;

    if (BT_SINK_SRV_CUSTOM_DB_DEV_INFO == info_type) {
        group_name = BT_SINK_SRV_NVDM_GROUP;
        item_name = BT_SINK_SRV_NVDM_ITEM_DEV_INFO;
    } else if (BT_SINK_SRV_CUSTOM_DB_SCAN_INFO == info_type) {
        group_name = BT_SINK_SRV_NVDM_GROUP;
        item_name = BT_SINK_SRV_NVDM_ITEM_SCAN;
    } else if (BT_SINK_SRV_CUSTOM_DB_SDAP_INFO == info_type) {
        group_name = BT_SINK_SRV_NVDM_GROUP;
        item_name = BT_SINK_SRV_NVDM_ITEM_SDAP;
    }

    if (NULL != group_name) {
        if (read) {
            result = nvdm_read_data_item(group_name,
                                         item_name,
                                         buffer,
                                         &size);
        } else {
            result = nvdm_write_data_item(group_name,
                                          item_name,
                                          NVDM_DATA_ITEM_TYPE_RAW_DATA,
                                          buffer,
                                          size);
        }
    }

    bt_sink_srv_report("[Sink][DB] RW NVDM, is_read:%d, result:%d", read, result);

    return (NVDM_STATUS_OK == result ? true : false);
}
#endif /* __BT_SINK_SRV_NVDM_SUPPORT__ */

bt_sink_srv_custom_db_type_t bt_sink_srv_db_custom_is_special_type(bt_sink_srv_db_t *db_p)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_custom_db_type_t info_type = BT_SINK_SRV_CUSTOM_DB_MAX;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (bt_sink_srv_strfind(db_p->name_p, BT_SINK_SRV_DB_DEVICE_INFO) != NULL) {
        info_type = BT_SINK_SRV_CUSTOM_DB_DEV_INFO;
    } else if (bt_sink_srv_strfind(db_p->name_p, BT_SINK_SRV_DB_SCAN_INFO) != NULL) {
        info_type = BT_SINK_SRV_CUSTOM_DB_SCAN_INFO;
    } else if (bt_sink_srv_strfind(db_p->name_p, BT_SINK_SRV_DB_SDAP_INFO) != NULL) {
        info_type = BT_SINK_SRV_CUSTOM_DB_SDAP_INFO;
    }

    return info_type;
}

bool bt_sink_srv_db_custom_read(bt_sink_srv_db_t *db_p)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_custom_db_type_t info_type = bt_sink_srv_db_custom_is_special_type(db_p);
    bool result = false;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (BT_SINK_SRV_CUSTOM_DB_MAX != info_type) {
#if defined(WIN32)
        result = bt_sink_srv_db_read_files(info_type, (uint8_t *)db_p->buffer_p, db_p->size * db_p->count);
#elif defined( __BT_SINK_SRV_NVDM_SUPPORT__ )
        result = bt_sink_srv_db_rw_nvdm(info_type, (uint8_t *)db_p->buffer_p, db_p->size * db_p->count, true);
#endif

        if (!result) {
            db_p->used = 0;
            bt_sink_srv_memset(db_p->buffer_p, 0, db_p->size * db_p->count);
#if defined(WIN32)
            result = bt_sink_srv_db_wirte_files(info_type, (uint8_t *)db_p->buffer_p, db_p->size * db_p->count);
#elif defined( __BT_SINK_SRV_NVDM_SUPPORT__ )
            result = bt_sink_srv_db_rw_nvdm(info_type, (uint8_t *)db_p->buffer_p, db_p->size * db_p->count, false);
#endif
        }
    }
    return result;
}

bool bt_sink_srv_db_custom_write(bt_sink_srv_db_t *db_p)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_custom_db_type_t info_type = bt_sink_srv_db_custom_is_special_type(db_p);
    bool result = false;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (BT_SINK_SRV_CUSTOM_DB_MAX != info_type) {
#if defined(WIN32)
        result = bt_sink_srv_db_wirte_files(info_type, (uint8_t *)db_p->buffer_p, db_p->size * db_p->count);
#elif defined( __BT_SINK_SRV_NVDM_SUPPORT__ )
        result = bt_sink_srv_db_rw_nvdm(info_type, (uint8_t *)db_p->buffer_p, db_p->size * db_p->count, false);
#endif
    }
    return result;
}

