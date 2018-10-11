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

/*****************************************************************************
 *
 * Description:
 * ------------
 * This file implements dogp adaptor service main function
 *
 ****************************************************************************/

#ifndef __BLE_DOGP_ADP_SERVICE_H__
#define __BLE_DOGP_ADP_SERVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "bt_type.h"

/************************************************
*   structures
*************************************************/
typedef uint16_t ble_dogp_adp_event_t;
#define BLE_DOGP_ADP_EVENT_CONNECT_IND          0
#define BLE_DOGP_ADP_EVENT_DISCONNECT_IND       1
#define BLE_DOGP_ADP_EVENT_READY_TO_READ_IND    2
#define BLE_DOGP_ADP_EVENT_READY_TO_WRITE_IND   3

typedef uint8_t ble_dogp_adp_remote_device_type_t;
#define BLE_DOGP_ADP_REMOTE_DEVICE_ANDROID      0
#define BLE_DOGP_ADP_REMOTE_DEVICE_IOS          1

#define BLE_ADDR_LEN      6   /**< Bluetooth Device Address Length defined in the specification. */

/**
 *  @brief      Address type, defined in spec.
 */
typedef uint8_t ble_addr_t[BLE_ADDR_LEN];   /**< Bluetooth Device Address defined in the specification. */

/**
 * structure for BLE_DOGP_ADP_EVENT_CONNECT_IND
 */
typedef struct {
    uint8_t        conn_result;     /**connect result, 0 = success, 1 = fail*/
    ble_addr_t     bdaddr;          /**remote device ble adress*/
    uint16_t       conn_handle;     /**connection id*/
} ble_dogp_adp_connect_t;

/**
 * structure for BLE_DOGP_ADP_EVENT_DISCONNECT_IND
 */
typedef struct {
    uint8_t  disconn_result;        /**disconnect result, 0 = success, 1 = fail*/
    uint16_t conn_handle;           /**connection id*/
} ble_dogp_adp_disconnect_t;

/**
 * structure for BLE_DOGP_ADP_EVENT_READY_TO_WRITE_IND
 */
typedef struct {
    ble_addr_t      bdaddr;         /**remote device ble adress*/
    uint16_t        conn_handle;    /**connection id*/
} ble_dogp_adp_ready_to_read_t;

/**
 * structure for BLE_DOGP_ADP_EVENT_READY_TO_READ_IND
 */
typedef struct {
    ble_addr_t     bdaddr;          /**remote device ble adress*/
    uint16_t       conn_handle;     /**connection id*/
} ble_dogp_adp_ready_to_write_t;


/**
 * define dogp app callback functions
 */
typedef void (*ble_dogp_adp_common_callback_t)(ble_dogp_adp_event_t event, void *callback_param);

/************************************************
*   static utilities
*************************************************/
/**
 *  @brief Function for application to register event handler.
*
*  @param[in]	app_callback		Applications' evnet calback.
*
*  @return	int32_t			0 means success, -1 means failed.
*/
int32_t ble_dogp_adp_init(ble_dogp_adp_common_callback_t app_callback);

/**
 *  @brief Function for application to deregister event handler.
*
*  @param[in]	app_callback		Applications' evnet calback.
*
*  @return	int32_t			0 means success, -1 means failed.
*/
int32_t ble_dogp_adp_deinit(ble_dogp_adp_common_callback_t app_callback);

/**
 *  @brief Function for application to register event handler.
*
*  @param[in]	conn_id			    connection handle.
*  @param[in]	buffer			    application's buffer to save data.
*  @param[in]	size			    length of the application's data read request.
*
*  @return	uint32_t	            the length of application actrually readed data.
*/
uint32_t ble_dogp_adp_read_data(uint16_t conn_handle, uint8_t *buffer, uint32_t size);

/**
 *  @brief Function for application to register event handler.
*
*  @param[in]	conn_id			    connection handle.
*  @param[in]	buffer			    application's data write buffer.
*  @param[in]	size			    length of the application's data write request.
*
*  @return	uint32_t		    the length of application actrually writed data.
*/
uint32_t ble_dogp_adp_write_data(uint16_t conn_handle, uint8_t *buffer, uint32_t size);

/**
 *  @brief Function for application to set remote device's type, android or ios device.
*
*  @param[in]	conn_id              connection handle.
*  @param[in]	ble_dogp_adp_remote_device_type_t
*
*  @return	void
*/
void ble_dogp_adp_set_remote_device_type(uint16_t conn_handle, ble_dogp_adp_remote_device_type_t type);

bt_status_t ble_dogp_adp_on_ble_evt(bt_msg_type_t msg, void *buff);

#if 0
/**for Ut*/
void ble_dogp_ut_main(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /*__BLE_DOGP_ADP_SERVICE_H__*/



