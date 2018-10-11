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

#ifndef __BT_SINK_SRV_DB_CUSTOM_H__
#define __BT_SINK_SRV_DB_CUSTOM_H__
#include <stdint.h>
#include <stdbool.h>
#include "bt_sink_srv_db.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BT_SINK_SRV_DB_FILE "devinfo.db"
#define BT_SINK_SRV_SCAN_DB_FILE "devscaninfo.db"
#define BT_SINK_SRV_DB_COD_FILE "devcod.db"
#define BT_SINK_SRV_SDAP_DEVDB_FILE "dev_sdap.db"
#define BT_SINK_SRV_DB_PATH "d:\\@bt"

#define BT_SINK_SRV_NVDM_GROUP "BT_SINK"
#define BT_SINK_SRV_NVDM_ITEM_DEV_INFO "devinfo"
#define BT_SINK_SRV_NVDM_ITEM_SCAN "scan"
#define BT_SINK_SRV_NVDM_ITEM_SDAP "sdap"

typedef enum {
    BT_SINK_SRV_CUSTOM_DB_DEV_INFO,
    BT_SINK_SRV_CUSTOM_DB_SCAN_INFO,
    BT_SINK_SRV_CUSTOM_DB_SDAP_INFO,
    BT_SINK_SRV_CUSTOM_DB_MAX,
} bt_sink_srv_custom_db_type_t;
bt_sink_srv_custom_db_type_t bt_sink_srv_db_custom_is_special_type(bt_sink_srv_db_t *db);
bool bt_sink_srv_db_custom_read(bt_sink_srv_db_t *db);
bool bt_sink_srv_db_custom_write(bt_sink_srv_db_t *db);

#ifdef __cplusplus
}
#endif

#endif /* __BT_SINK_SRV_DB_CUSTOM_H__ */
