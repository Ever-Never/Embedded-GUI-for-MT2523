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

#include "ble_pxp_main.h"
#include "ble_pxp_app.h"


ble_pxp_context_t g_ble_pxp_ctx;


ble_pxp_context_t *ble_pxp_get_context(void)
{
    return &g_ble_pxp_ctx;
}


static ble_gatt_srv_status_t ble_pxp_gatt_srv_handle(ble_gatt_srv_event_t event,
    void *para, void *user_data)
{
    ble_gatt_srv_status_t ret = BLE_GATT_SRV_STATUS_OK;
    ble_gatt_char_alter_level_t *char_al = NULL;
    ble_pxp_show_type_t scr_type = BLE_PXP_SHOW_CONNECT;

    ble_gatt_report("[pxp]srv_handle(s)--ent: 0x%x\n", event);

    switch (event) {
        case BLE_GATT_SRV_LLS_AL_WRITE: {
            char_al = (ble_gatt_char_alter_level_t *)para;
            ble_gatt_report("[pxp]srv_handle(al_w)--hd: 0x%x, al: %d", char_al->handle, char_al->al);
            if (1 == char_al->al || 2 == char_al->al) {
                scr_type = BLE_PXP_SHOW_ALERT;
            } else {
                scr_type = BLE_PXP_SHOW_CONNECT;
            }
            ble_pxp_show_screen(scr_type);

            break;
        }

        case BLE_GATT_SRV_LE_CONNECT: {
            ble_pxp_show_screen(BLE_PXP_SHOW_CONNECT);
            break;
        }

        case BLE_GATT_SRV_LE_DISCONNECT: {
            ble_pxp_show_screen(BLE_PXP_SHOW_DISCONNECT);
            break;
        }

        default:
            break;
    }

    ble_gatt_report("[pxp]srv_handle(e)--ret: %d\n", ret);
    return ret;
}


void ble_pxp_init(void)
{
    int32_t ret = 0;

    memset(&g_ble_pxp_ctx, 0x00, sizeof(ble_pxp_context_t));

    ble_gatt_srv_init();

    ret = ble_gatt_srv_register(ble_pxp_gatt_srv_handle, &g_ble_pxp_ctx);

    ble_gatt_report("[pxp]init--ret: %d\n", ret);

#if defined(__GNUC__)
    printf("Version: %s\n", "__GNUC__");
#endif

#if defined(__ARMCC_VERSION)
    printf("Version: %s\n", "__ARMCC_VERSION");
#endif

#if defined(__ICCARM__)
    printf("Version: %s\n", "__ICCARM__");
#endif
}


void ble_pxp_deinit(void)
{
    int32_t ret = 0;

    ret = ble_gatt_srv_deregister(ble_pxp_gatt_srv_handle, &g_ble_pxp_ctx);

    ble_gatt_report("[pxp]deinit--ret: %d\n", ret);
}

