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

#include "ble_gatt_srv_internal.h"
#include "ble_gatt_srv.h"
#include "ble_gatt_utils.h"
#include "ble_gatt_connect.h"
#include "ble_lls.h"

static ble_gatt_srv_user_t g_ble_gatt_srv_user[BLE_GATT_SRV_MAX_USER];

static ble_gatt_srv_context_t g_ble_gatt_srv_ctx;


ble_gatt_srv_context_t *ble_gatt_srv_get_context(void)
{
    return &g_ble_gatt_srv_ctx;
}


void ble_gatt_srv_notify(ble_gatt_srv_event_t event, void *para)
{
    int32_t i = 0;
    ble_gatt_srv_context_t *gatt_srv = NULL;
    ble_gatt_srv_user_t *user = NULL;

   gatt_srv = ble_gatt_srv_get_context();
   user = gatt_srv->user;

   for (i = 0; i < BLE_GATT_SRV_MAX_USER; ++i) {
       ble_gatt_report("[gatt][srv]notify--i: %d, flag: 0x%x, cb_func: 0x%x\n",
            i, user->uflag, user->cb_func);
       if ((user->uflag & BLE_GATT_SRV_USER_FLAG_USED)) {
            user->cb_func(event, para, user->user_data);
       }
       ++user;
   }
}


/* extern API */
ble_gatt_srv_status_t ble_gatt_srv_init(void)
{
    ble_gatt_srv_status_t ret = BLE_GATT_SRV_STATUS_OK;

    ble_gatt_memset(&g_ble_gatt_srv_user[0], 0x00, sizeof(ble_gatt_srv_user_t) * BLE_GATT_SRV_MAX_USER);
    ble_gatt_memset(&g_ble_gatt_srv_ctx, 0x00, sizeof(ble_gatt_srv_context_t));

    g_ble_gatt_srv_ctx.user = g_ble_gatt_srv_user;

    /* connect manager */
    ble_gatt_connect_init();
    /* lls init */
    ble_lls_init();

    ble_gatt_report("[gatt][srv]init");
    return ret;
}


ble_gatt_srv_status_t ble_gatt_srv_register(ble_gatt_srv_callback_t cb_func,
    void *user_data)
{
    int32_t i = 0;
    ble_gatt_srv_status_t ret = BLE_GATT_SRV_STATUS_OK;
    ble_gatt_srv_context_t *gatt_srv = NULL;
    ble_gatt_srv_user_t *freed = NULL;

    gatt_srv = ble_gatt_srv_get_context();
    
    for (i = 0; i < BLE_GATT_SRV_MAX_USER; ++i) {
        if ((gatt_srv->user[i].uflag & BLE_GATT_SRV_USER_FLAG_USED)
            && (gatt_srv->user[i].cb_func == cb_func)
            && (gatt_srv->user[i].user_data == user_data)) {
            ret = BLE_GATT_SRV_STATUS_REGISTERED;
            break;
        }
    }

    if (!cb_func) {
        ret = BLE_GATT_SRV_STATUS_PARAM;
    }

    if (BLE_GATT_SRV_STATUS_OK == ret) {
        /* alloc free user */
        for (i = 0; i < BLE_GATT_SRV_MAX_USER; ++i) {
            if (gatt_srv->user[i].uflag & BLE_GATT_SRV_USER_FLAG_USED) {
                ;
            } else {
                freed = &(gatt_srv->user[i]);
                break;
            }
        }
    }

    if (freed) {
        freed->cb_func = cb_func;
        freed->user_data = user_data;
        freed->uflag |= BLE_GATT_SRV_USER_FLAG_USED;

        ret = BLE_GATT_SRV_STATUS_OK;
    } else {
        ret = BLE_GATT_SRV_STATUS_EXHAUST;
    }

    ble_gatt_report("[gatt][srv]register--cb: 0x%x, user: 0x%x, ret: %d\n", 
        cb_func, user_data, ret);

    return ret;
}


ble_gatt_srv_status_t ble_gatt_srv_deregister(ble_gatt_srv_callback_t cb_func,
    void *user_data)
{
    int32_t i = 0;
    ble_gatt_srv_status_t ret = BLE_GATT_SRV_STATUS_OK;
    ble_gatt_srv_context_t *gatt_srv = NULL;
    ble_gatt_srv_user_t *user = NULL, *used = NULL;

    gatt_srv = ble_gatt_srv_get_context();
    user = gatt_srv->user;

    for (i = 0; i < BLE_GATT_SRV_MAX_USER; ++i) {
        if ((user->uflag & BLE_GATT_SRV_USER_FLAG_USED)
            && (user->cb_func == cb_func)
            && (user->user_data == user_data)) {
            used = user;
            break;
        }
        ++user;
    }

    if (used) {
        ble_gatt_memset(used, 0x00, sizeof(ble_gatt_srv_user_t));
        ret = BLE_GATT_SRV_STATUS_OK;
    } else {
        ret = BLE_GATT_SRV_STATUS_FAIL;
    }

    ble_gatt_report("[gatt_srv]deregister--cb: 0x%x, user: 0x%x, ret: %d\n",
        cb_func, user_data, ret);

    return ret;
}

