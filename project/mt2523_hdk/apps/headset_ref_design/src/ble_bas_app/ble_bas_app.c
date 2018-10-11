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

#include "bt_gap_le.h"
#include "bt_callback_manager.h"
#include "ble_bas.h"
#include "ble_bas_app.h"
#include "syslog.h"
#ifdef MTK_SMART_BATTERY_ENABLE
#include "battery_management.h"
#endif

//#define BAS_UT
#ifdef BAS_UT
/*UT part*/
#include "FreeRTOS.h"
#include "timers.h"

#define BLE_BAS_TIMER_PERIOD       60000 /** 1 minutes. */
TimerHandle_t xTimer_BAS;

static void ble_bas_app_ut_timerout(TimerHandle_t xTimer);
static void ble_bas_app_ut(void);
#endif
/* Create the log control block as user wishes. Here we use 'BT_IAS' as module name.
 * User needs to define their own log control blocks as project needs.
 * Please refer to the log dev guide under /doc folder for more details.
 */
log_create_module(BAS_APP, PRINT_LEVEL_INFO);

static uint8_t g_battery_level = 0;
static ble_bas_buffer_t g_bas_buffer[BLE_BAS_CONNECTION_MAX];


static bt_status_t ble_bas_app_save_connection_info(void *buff);
static void ble_bas_app_clear_all_connection_info(void);
static bt_status_t ble_bas_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff);
static bt_status_t ble_bas_app_event_callback_register(void);
static void ble_bas_app_write_event_callback(ble_bas_event_t event, uint16_t conn_handle, uint16_t data);
static uint8_t ble_bas_app_read_event_callback(ble_bas_event_t event, uint16_t conn_handle);
#ifndef BAS_UT
static bt_status_t ble_bas_app_battery_level_change_notify(uint16_t conn_handle, uint8_t battery_level);
#endif
static void ble_bas_app_battery_level_update(uint8_t battery_level);


#ifdef BAS_UT
static void ble_bas_app_ut_timerout(TimerHandle_t xTimer)
{
    LOG_I(BAS_APP, "%s: enter\r\n", __FUNCTION__);  

    if (pdFALSE != xTimerIsTimerActive(xTimer_BAS)) {
        xTimerStop(xTimer_BAS, 0);
        xTimerDelete(xTimer_BAS, 0);
    }
    
    if (g_battery_level >= 1) {  
        g_battery_level = g_battery_level -1;
        ble_bas_app_ut();
    }
}

static void ble_bas_app_ut(void)
{
    uint8_t battery;
    LOG_I(BAS_APP, "%s: enter\r\n", __FUNCTION__);  

    battery = g_battery_level - 1;     
    ble_bas_app_update_battery_capacity(battery);

    xTimer_BAS = xTimerCreate("BAS_TIMER", /* Just a text name, not used by the kernel. */
                              BLE_BAS_TIMER_PERIOD / portTICK_PERIOD_MS, /* The timer period in ticks. */
                              pdFALSE,
                              (void *)0,
                              ble_bas_app_ut_timerout);
        
    if (xTimer_BAS == NULL) {
        LOG_I(BAS_APP,  "init create xTimer_BAS fail\r\n");
    }
    
    LOG_I(BAS_APP,  "BAS_TIMER:%d\r\n", xTimer_BAS);
    xTimerStart(xTimer_BAS, 0);
}
#endif

void ble_bas_app_update_battery_capacity(int32_t capacity)
{    
    LOG_I(BAS_APP, "%s: capacity(0x%04x)\r\n", __FUNCTION__, capacity);  

    if ((capacity >= 0) && (capacity <= 100)) {
        if (capacity != g_battery_level) {
            g_battery_level = (uint8_t)capacity;
            ble_bas_app_battery_level_update(g_battery_level);
        }
    }
}

static bt_status_t ble_bas_app_save_connection_info(void *buff)
{
    uint8_t i, j;
    bt_status_t status;
    
    LOG_I(BAS_APP, "%s: enter\r\n", __FUNCTION__);
    bt_gap_le_connection_ind_t *conn_ind = (bt_gap_le_connection_ind_t *)buff;

#ifdef MTK_SMART_BATTERY_ENABLE
    int32_t capacity = 0;
    capacity = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);
    if ((capacity >= 0) && (capacity <= 100)) {
        g_battery_level = (uint8_t)capacity;
        LOG_I(BAS_APP, "Get battery success, bas_level is %d\r\n", g_battery_level); 
    }
#endif
    /** reconnect, to notify the current battery level. */
    for (i = 0; i< BLE_BAS_CONNECTION_MAX; i++) {
        if (conn_ind->connection_handle == g_bas_buffer[i].conn_handle) {  
            LOG_I(BAS_APP, "reconnect, conn_handle is 0x%04x\r\n", conn_ind->connection_handle); 
            ble_bas_notify_battery_level(g_bas_buffer[i].conn_handle, g_battery_level);
            break;
        }
    }

    /** first connect, to save connection info. */
    if (i == BLE_BAS_CONNECTION_MAX) {
        LOG_I(BAS_APP, "First connect, conn_handle is 0x%04x\r\n", conn_ind->connection_handle); 
        for (j =0; j< BLE_BAS_CONNECTION_MAX; j++) {
            if (0 == g_bas_buffer[j].conn_handle) {
                memset(&(g_bas_buffer[j]),0x00, sizeof(ble_bas_buffer_t));
                g_bas_buffer[j].conn_handle = conn_ind->connection_handle;
                break;
            }
        }
        if (j == BLE_BAS_CONNECTION_MAX)     
            LOG_I(BAS_APP, "Reach maximum connection, no empty buffer!\r\n");
            status = BT_STATUS_OUT_OF_MEMORY;
    }

    return status;
}

static void ble_bas_app_clear_all_connection_info(void)
{
    uint8_t i;
    LOG_I(BAS_APP, "%s: enter\r\n", __FUNCTION__);

    for (i = 0; i< BLE_BAS_CONNECTION_MAX ; i++) {   
        memset(&(g_bas_buffer[i]),0x00, sizeof(ble_bas_buffer_t));
    }
}

static bt_status_t ble_bas_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    LOG_I(BAS_APP, "%s: status(0x%04x), msg(0x%04x)", __FUNCTION__, status, msg);

    if(status != BT_STATUS_SUCCESS) {
       return BT_STATUS_SUCCESS;
    }

     switch (msg)
     {
         case BT_GAP_LE_CONNECT_IND: 
             ble_bas_app_save_connection_info(buff);
             break;  
                         
         case BT_GAP_LE_DISCONNECT_IND: 
            break;
            
         default:
             break;
     }
     return BT_STATUS_SUCCESS;
}

static uint8_t ble_bas_app_read_event_callback(ble_bas_event_t event, uint16_t conn_handle)
{
    uint8_t i; 

    LOG_I(BAS_APP, "%s: enter\r\n", __FUNCTION__);
    for (i = 0; i< BLE_BAS_CONNECTION_MAX ; i++) {
        if (conn_handle == g_bas_buffer[i].conn_handle) {
            if (event == BLE_BAS_EVENT_BATTRY_LEVEL_READ) {  
            #ifdef BAS_UT
                // just for UT
                g_battery_level += 10;
                if (g_battery_level >= 100) {
                    g_battery_level = 100;
                }
            #endif
                return g_battery_level;
            } else if (event == BLE_BAS_EVENT_CCCD_READ) {
                return g_bas_buffer[i].notify_enabled;
            }
            return 0;
        }
    }
    if (i == BLE_BAS_CONNECTION_MAX) {
        LOG_I(BAS_APP, "Unknown connetion handle!\r\n");
    }
    return 0;
}

static void ble_bas_app_write_event_callback(ble_bas_event_t event, uint16_t conn_handle, uint16_t data)
{
    uint8_t i; 

    LOG_I(BAS_APP, "%s: enter\r\n", __FUNCTION__);
    for (i = 0; i< BLE_BAS_CONNECTION_MAX ; i++) {
        if (conn_handle == g_bas_buffer[i].conn_handle) {
            g_bas_buffer[i].notify_enabled = data;
            LOG_I(BAS_APP, "%s: i is %d, notify_enabled is %d\r\n", __FUNCTION__, i, g_bas_buffer[i].notify_enabled);
            if (BLE_BAS_CCCD_NOTIFICATION == g_bas_buffer[i].notify_enabled) {
            #ifndef BAS_UT
                ble_bas_app_battery_level_change_notify(conn_handle, g_battery_level);
            #else
                ble_bas_app_ut();
            #endif
            }

            break;
        }
    }
    if (i == BLE_BAS_CONNECTION_MAX) {
        LOG_I(BAS_APP, "Unknown connetion handle!\r\n");
    }
}

static bt_status_t ble_bas_app_event_callback_register(void)
{  
    LOG_I(BAS_APP, "%s: enter\r\n", __FUNCTION__);  
    return bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_GAP, (void *)ble_bas_app_event_callback);   
}

uint8_t ble_bas_read_callback(ble_bas_event_t event, bt_handle_t conn_handle)
{
    return ble_bas_app_read_event_callback(event, conn_handle);
}

void ble_bas_write_callback(ble_bas_event_t event, bt_handle_t conn_handle, void *data)
{
    uint16_t cccd_value = 0;
    cccd_value = *(uint16_t *)data;

    LOG_I(BAS_APP, "%s:cccd_value is %d\r\n", __FUNCTION__, cccd_value); 
    ble_bas_app_write_event_callback(event, conn_handle, cccd_value);
}

bt_status_t ble_bas_app_init(void)
{
    bt_status_t status;

    LOG_I(BAS_APP, "%s: enter\r\n", __FUNCTION__);  
    
    ble_bas_app_clear_all_connection_info();
    status = ble_bas_app_event_callback_register();
    LOG_I(BAS_APP, "ble_bas_app_init register callback success! \r\n");  
    return status;
}

#ifndef BAS_UT
static bt_status_t ble_bas_app_battery_level_change_notify(uint16_t conn_handle, uint8_t battery_level)
{
    uint8_t i;
    bt_status_t status;
    LOG_I(BAS_APP, "%s: enter\r\n", __FUNCTION__);  

    for (i = 0; i<BLE_BAS_CONNECTION_MAX; i++) {
        if ((conn_handle > 0) && (conn_handle == g_bas_buffer[i].conn_handle)) {
            if (BLE_BAS_CCCD_NOTIFICATION == g_bas_buffer[i].notify_enabled ) {
                status = ble_bas_notify_battery_level(conn_handle, battery_level);
                LOG_I(BAS_APP, "%s: success, conn_handle(0x%04x), battery is %d\r\n", __FUNCTION__, conn_handle, battery_level);
                return status;
            }
        }
    }
    return BT_STATUS_FAIL;
}
#endif

/**< update all remote devices, which configed CCCD. */
static void ble_bas_app_battery_level_update(uint8_t battery_level)
{
    uint8_t i;
    LOG_I(BAS_APP, "%s: enter\r\n", __FUNCTION__);  

    for (i=0; i<BLE_BAS_CONNECTION_MAX; i++) {
        if (0 != g_bas_buffer[i].conn_handle) {
            if (BLE_BAS_CCCD_NOTIFICATION == g_bas_buffer[i].notify_enabled ) {
                ble_bas_notify_battery_level(g_bas_buffer[i].conn_handle, battery_level);
                LOG_I(BAS_APP, "%s: success, i is %d, battery is %d\r\n", __FUNCTION__, i, battery_level);  
            }
        }
    }
}


