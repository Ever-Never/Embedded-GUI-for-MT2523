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

#include "ble_fmp_main.h"


ble_fmp_context_t g_ble_fmp_ctx;


/*****************************************************************************
* FUNCTION
*  ble_fmp_get_context
* DESCRIPTION
* Get FMP context
* PARAMETERS
*  void
* RETURNS
*  ble_fmp_context_t
*****************************************************************************/
ble_fmp_context_t *ble_fmp_get_context(void)
{
    return &g_ble_fmp_ctx;
}


/*****************************************************************************
* FUNCTION
*  ble_fmp_gatt_srv_handle
* DESCRIPTION
* Handle le event
* PARAMETERS
*  event         [IN]        Event message ID
*  para          [IN]        Event para
*  user_data     [IN]        Event user data
* RETURNS
*  ble_gatt_srv_status_t
*****************************************************************************/
static ble_gatt_srv_status_t ble_fmp_gatt_srv_handle(ble_gatt_srv_event_t event,
    void *para, void *user_data)
{
    ble_gatt_srv_status_t ret = BLE_GATT_SRV_STATUS_OK;
    ble_gatt_char_alter_level_t *char_al = NULL;

    ble_gatt_report("[fmp]srv_handle(s)--ent: 0x%x\n", event);

    switch (event) {
        case BLE_GATT_SRV_IAS_AL_WRITE: {
            char_al = (ble_gatt_char_alter_level_t *)para;
            ble_gatt_report("[fmp]srv_handle(al_w)--hd: 0x%x, al: %d", char_al->handle, char_al->al);
            break;
        }

        default:
            break;
    }

    ble_gatt_report("[fmp]srv_handle(e)--ret: %d\n", ret);
    return ret;
}


/*****************************************************************************
* FUNCTION
*  ble_fmp_init
* DESCRIPTION
* FMP init
* PARAMETERS
*  void
* RETURNS
*  void
*****************************************************************************/
void ble_fmp_init(void)
{
    int32_t ret = 0;

    memset(&g_ble_fmp_ctx, 0x00, sizeof(ble_fmp_context_t));

    ble_gatt_srv_init();

    ret = ble_gatt_srv_register(ble_fmp_gatt_srv_handle, &g_ble_fmp_ctx);

    ble_gatt_report("[fmp]init--ret: %d\n", ret);

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


/*****************************************************************************
* FUNCTION
*  ble_fmp_deinit
* DESCRIPTION
* FMP deinit
* PARAMETERS
*  void
* RETURNS
*  void
*****************************************************************************/
void ble_fmp_deinit(void)
{
    int32_t ret = 0;

    ret = ble_gatt_srv_deregister(ble_fmp_gatt_srv_handle, &g_ble_fmp_ctx);

    ble_gatt_report("[fmp]deinit--ret: %d\n", ret);
}

