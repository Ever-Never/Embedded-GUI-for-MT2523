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

#ifndef __BLE_GATT_COMMON_H__
#define __BLE_GATT_COMMON_H__

#include "bt_uuid.h"
#include "bt_system.h"
#include "bt_gattc.h"
#include "bt_gatt.h"
#include "bt_gatts.h"
#include "bt_gap_le.h"


#ifdef __cplusplus
#define BLE_EXTERN_C_BEGIN extern "C" {
#else
#define BLE_EXTERN_C_BEGIN
#endif

#ifdef __cplusplus
#define BLE_EXTERN_C_END }
#else
#define BLE_EXTERN_C_END
#endif

/********************
#if _MSC_VER >= 1500
#define BLE_PACKED(...) __pragma(pack(push, 1)) __VA_ARGS__ __pragma(pack(pop))
#elif defined(__GNUC__)
#define BLE_PACKED(...) __VA_ARGS__ __attribute__((__packed__))
#define BT_ALIGNMENT4(...) __VA_ARGS__ __attribute__((aligned(4)))
#elif defined(__ARMCC_VERSION)
#pragma anon_unions
#define BLE_PACKED(...) __VA_ARGS__ __attribute__((__packed__))
#define BT_ALIGNMENT4(...) __VA_ARGS__ __attribute__((aligned(4)))
#elif defined(__ICCARM__)
#define BLE_PACKED(...) __packed __VA_ARGS__
#define BT_ALIGNMENT4(...) _Pragma("data_alignment=4") __VA_ARGS__
#else
#error "Unsupported Platform"
#endif
******************/

#if defined(__GNUC__)
#define BLE_PACKED(...) __VA_ARGS__ __attribute__((__packed__))
#define BT_ALIGNMENT4(...) __VA_ARGS__ __attribute__((aligned(4)))
#elif defined(__ARMCC_VERSION)
#pragma anon_unions
#define BLE_PACKED(...) __VA_ARGS__ __attribute__((__packed__))
#define BT_ALIGNMENT4(...) __VA_ARGS__ __attribute__((aligned(4)))
#elif defined(__ICCARM__)
#define BLE_PACKED(...) __packed __VA_ARGS__
#define BT_ALIGNMENT4(...) _Pragma("data_alignment=4") __VA_ARGS__
#else
#error "Unsupported Platform"
#endif

//BLE_EXTERN_C_BEGIN


/* GAP Service */
#define BLE_GAP_START_HANDLE                (0x0001)
#define BLE_GAP_END_HANDLE                  (0x000c)
#define BLE_GAP_REKS                        (7)

/* GAP value handle */
#define BLE_GAP_VAL_HD_DEV_NAME             (0x0004)
#define BLE_GAP_VAL_HD_APP                  (0x000a)
#define BLE_GAP_VAL_HD_PPCP                 (0x000c)

/* GATT Service */
#define BLE_GATT_START_HANDLE               (0x0011)
#define BLE_GATT_END_HANDLE                 (0x0013)
#define BLE_GATT_REKS                       (7)

/* GATT value handle */
#define BLE_GAP_VAL_HD_SC                   (0x0013)


/* Immediate Alert Service */
#define BLE_IAS_START_HANDLE                (0x0600)
#define BLE_IAS_END_HANDLE                  (0x0602)
#define BLE_IAS_REKS                        (0)

/* Immediate Alert value handle */
#define BLE_IAS_VAL_HD_AL                   (0x0602)


/* Link Loss Service */
#define BLE_LLS_START_HANDLE                (0x0700)
#define BLE_LLS_END_HANDLE                  (0x0702)
#define BLE_LLS_REKS                        (0)

/* Link Loss value handle */
#define BLE_LLS_VAL_HD_AL                   (0x0702)



#define BT_GATT_UUID16_IMMEDIATE_ALERT_SERVICE                (0x1802)      /**< Immediate alert service. */
#define BT_GATT_UUID16_LINK_LOSS_SERVICE                      (0x1803)      /**< Immediate alert service. */


#define BT_SIG_UUID16_ALERT_LEVEL                             (0x2A06)      /**< Alert level. */




#define BLE_GATT_MAX_LINK                   (2)


/* debug trace error code */
#define BLE_GATT_ERR_SUCCESS_7TH            (7)
#define BLE_GATT_ERR_SUCCESS_6TH            (6)
#define BLE_GATT_ERR_SUCCESS_5TH            (5)
#define BLE_GATT_ERR_SUCCESS_4TH            (4)
#define BLE_GATT_ERR_SUCCESS_3RD            (3)
#define BLE_GATT_ERR_SUCCESS_2ND            (2)
#define BLE_GATT_ERR_SUCCESS_1ST            (1)
#define BLE_GATT_ERR_SUCCESS_OK             (0)
#define BLE_GATT_ERR_FAIL_1ST               (-1)
#define BLE_GATT_ERR_FAIL_2ND               (-2)
#define BLE_GATT_ERR_FAIL_3RD               (-3)
#define BLE_GATT_ERR_FAIL_4TH               (-4)
#define BLE_GATT_ERR_FAIL_5TH               (-5)
#define BLE_GATT_ERR_FAIL_6TH               (-6)
#define BLE_GATT_ERR_FAIL_7TH               (-7)


/* GATT Common Service */
/* GAP */
extern const bt_gatts_service_t bt_if_gap_service;
/* GATT */
extern const bt_gatts_service_t bt_if_gatt_service;
/* add GATT Service declaration after this line  */

/* IAS */
extern const bt_gatts_service_t bt_if_ias_service;

/* LLS */
extern const bt_gatts_service_t bt_if_lls_service;



bt_status_t ble_app_event_handle(bt_msg_type_t msg, bt_status_t status,
    void *buff);

bt_status_t ble_gatt_event_handle(bt_msg_type_t msg, bt_status_t status,
    void *buff);

//BLE_EXTERN_C_END

#endif /* __BLE_GATT_COMMON_H__ */

