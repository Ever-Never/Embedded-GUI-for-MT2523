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

/* Kernel includes. */
#include "bt_os_layer_api.h"
#include "FreeRTOS.h"
#include "task.h"
//#include "timer.h"
#include "semphr.h"
#include "portmacro.h"
#include "queue.h"
#include <timers.h>
#include <string.h>
#include "syslog.h"
#include "hal_aes.h"
#include "mbedtls/md5.h"
#if !defined(__GNUC__)
#include "mbedtls/aes.h"
#endif
#include "bt_debug.h"
#include "hal_nvic.h"

#ifdef MTK_PORT_SERVICE_ENABLE
#include "serial_port.h"
#endif

#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager.h"
#endif
#ifdef HAL_GPT_MODULE_ENABLED
#include "hal_gpt.h"
#endif
#include "bt_hci_log.h"

static TimerHandle_t bt_rtos_timer = NULL; /**< Timer handler. */
static bt_os_layer_timer_expired_t bt_rtos_timer_cb;  /**< Timer callback function. */

extern int rand(void);
uint16_t bt_os_layer_generate_random(void)
{
    return rand();
}

void bt_os_layer_aes_encrypt(bt_os_layer_aes_buffer_t* encrypted_data, bt_os_layer_aes_buffer_t* plain_text, bt_os_layer_aes_buffer_t* key)
{
    #if defined(__GNUC__)
    BT_LOGI("BT", "bt_os_layer_aes_encrypt use hal version");
    hal_aes_ecb_encrypt((hal_aes_buffer_t*)encrypted_data, (hal_aes_buffer_t*)plain_text, (hal_aes_buffer_t*)key);
    #else
    mbedtls_aes_context context;
    mbedtls_aes_init(&context);
    mbedtls_aes_setkey_enc(&context, key->buffer, sizeof(bt_key_t) * 8);
    mbedtls_aes_crypt_ecb(&context, MBEDTLS_AES_ENCRYPT, (unsigned char *)plain_text->buffer, (unsigned char *)encrypted_data->buffer);
    mbedtls_aes_free(&context);
    BT_LOGI("BT", "bt_os_layer_aes_encrypt use mbed version");
    #endif
}

static void bt_os_layer_rtos_timer_os_expire(TimerHandle_t timer)
{
    if(bt_rtos_timer_cb != NULL) {
        bt_rtos_timer_cb();
    }
}

void bt_os_layer_init_timer(void)
{
    if(bt_rtos_timer == NULL) {
        bt_rtos_timer = xTimerCreate( "hb timer", 0xffff, pdFALSE, NULL, bt_os_layer_rtos_timer_os_expire);
        bt_rtos_timer_cb = NULL;
    }
}

void bt_os_layer_deinit_timer(void)
{
    if (bt_rtos_timer != NULL) {
        xTimerDelete(bt_rtos_timer, 0);
        bt_rtos_timer = NULL;
    }
}

void bt_os_layer_sleep_task(uint32_t ms)
{
    uint32_t time_length = ms/portTICK_PERIOD_MS;
    if(time_length > 0) {
        vTaskDelay(time_length);
    }
}

uint32_t bt_os_layer_get_current_task_id(void)
{
    return (uint32_t)xTaskGetCurrentTaskHandle();
}

//MUTEX LOCK
uint32_t bt_os_layer_create_mutex(void)
{
    return (uint32_t)xSemaphoreCreateRecursiveMutex();
}

void bt_os_layer_take_mutex(uint32_t mutex_id)
{
    if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        return;
    }
    xSemaphoreTakeRecursive((SemaphoreHandle_t)mutex_id, portMAX_DELAY);
}

void bt_os_layer_give_mutex(uint32_t mutex_id)
{
    if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        return;
    }
    xSemaphoreGiveRecursive((SemaphoreHandle_t)mutex_id);
}

void bt_os_layer_delete_mutex(uint32_t mutex_id)
{
    vSemaphoreDelete((SemaphoreHandle_t)mutex_id);
}

uint32_t bt_os_layer_create_semaphore()
{
    return (uint32_t)xSemaphoreCreateBinary();
}

void bt_os_layer_take_semaphore_from_isr(uint32_t semaphore_id)
{
    BaseType_t priorityTaskWoken;
    xSemaphoreTakeFromISR((SemaphoreHandle_t)semaphore_id, &priorityTaskWoken);
}

void bt_os_layer_take_semaphore(uint32_t semaphore_id)
{
    xSemaphoreTake((SemaphoreHandle_t)semaphore_id, portMAX_DELAY);
}

void bt_os_layer_give_semaphore_from_isr(uint32_t semaphore_id)
{
    BaseType_t priorityTaskWoken;
    xSemaphoreGiveFromISR((SemaphoreHandle_t)semaphore_id, &priorityTaskWoken);
    return;
}

void bt_os_layer_give_semaphore(uint32_t semaphore_id)
{
    xSemaphoreGive((SemaphoreHandle_t)semaphore_id);
}

void bt_os_layer_delete_semaphore(uint32_t semaphore_id)
{
    vSemaphoreDelete((SemaphoreHandle_t)semaphore_id);
}

uint32_t bt_os_layer_get_system_tick(void)
{
    return xTaskGetTickCount();
}

void bt_os_layer_register_timer_callback(bt_os_layer_timer_expired_t callback)
{
    bt_rtos_timer_cb = callback;
}

void bt_os_layer_start_timer(uint32_t ms)
{
    uint32_t time_length = ms/portTICK_PERIOD_MS + 1;
    if (bt_rtos_timer == NULL) {
        return;
    }
    if (bt_os_layer_is_timer_active() == 1) {
        bt_os_layer_stop_timer();
    }
    xTimerChangePeriod(bt_rtos_timer, time_length, portMAX_DELAY);
    xTimerReset(bt_rtos_timer, portMAX_DELAY);
}

void bt_os_layer_stop_timer(void)
{
    if  ((bt_rtos_timer != NULL) && (bt_os_layer_is_timer_active() == 1)) {
        xTimerStop(bt_rtos_timer, portMAX_DELAY);
    }
}

uint32_t bt_os_layer_is_timer_active(void)
{
    if ((bt_rtos_timer != NULL) && (xTimerIsTimerActive(bt_rtos_timer) != pdFALSE)) {
        return 1;
    }
    else {
        return 0;
    }
}

void bt_os_layer_disable_interrupt(void)
{
    taskDISABLE_INTERRUPTS();
}

void bt_os_layer_enable_interrupt(void)
{
    taskENABLE_INTERRUPTS();
}

void bt_os_layer_disable_system_sleep(void)
{

}

void bt_os_layer_enable_system_sleep(void)
{

}


void* bt_os_layer_memcpy(void* dest, const void* src, uint32_t size)
{
    return memcpy(dest, src, size);                
}

int bt_os_layer_memcmp(const void* buf1, const void* buf2, uint32_t size)
{
    return memcmp(buf1, buf2, size);                
}

void* bt_os_layer_memset(void* buf, uint8_t ch, uint32_t size)
{
    return memset(buf, ch, size);                
}

void* bt_os_layer_memmove(void* dest, const void* src, uint32_t size)
{
    return memmove(dest, src, size);                
}


void bt_os_layer_md5_init(bt_os_md5_context *ctx)
{
#ifdef MTK_MBEDTLS_CONFIG_FILE
    mbedtls_md5_init((mbedtls_md5_context *)ctx );
#endif
}

void bt_os_layer_md5_free(bt_os_md5_context *ctx)
{
#ifdef MTK_MBEDTLS_CONFIG_FILE
  mbedtls_md5_free((mbedtls_md5_context *)ctx );
#endif
}

void bt_os_layer_md5_clone(bt_os_md5_context *dst, const bt_os_md5_context *src)
{
#ifdef MTK_MBEDTLS_CONFIG_FILE
    mbedtls_md5_clone((mbedtls_md5_context *)dst, (const mbedtls_md5_context *)src );
#endif
}

void bt_os_layer_md5_starts(bt_os_md5_context *ctx) 
{
#ifdef MTK_MBEDTLS_CONFIG_FILE
    mbedtls_md5_starts((mbedtls_md5_context *)ctx );
#endif
}

void bt_os_layer_md5_process(bt_os_md5_context *ctx, const unsigned char data[64])
{
#ifdef MTK_MBEDTLS_CONFIG_FILE
   mbedtls_md5_process((mbedtls_md5_context *)ctx, data);
#endif
}

void bt_os_layer_md5_update(bt_os_md5_context *ctx, const unsigned char *input, unsigned int ilen)
{
#ifdef MTK_MBEDTLS_CONFIG_FILE
    mbedtls_md5_update((mbedtls_md5_context *)ctx, input, (size_t)ilen);
#endif
}

void bt_os_layer_md5_finish(bt_os_md5_context *ctx, unsigned char output[16])
{
#ifdef MTK_MBEDTLS_CONFIG_FILE
    mbedtls_md5_finish((mbedtls_md5_context *)ctx, output);
#endif
}

int16_t bt_os_layer_serial_port_open(uint16_t device, void *para, uint32_t *handle)
{
#ifdef MTK_PORT_SERVICE_ENABLE
	return serial_port_open((serial_port_dev_t) device, (serial_port_open_para_t *)para, (serial_port_handle_t*)handle);
#else
	return -1;
#endif
}


int16_t bt_os_layer_serial_port_close(uint32_t handle)
{
#ifdef MTK_PORT_SERVICE_ENABLE
    return serial_port_close((serial_port_handle_t) handle);
#else
    return -1;
#endif
}


int16_t bt_os_layer_serial_port_control(uint32_t handle, uint8_t command, void *para)
{
#ifdef MTK_PORT_SERVICE_ENABLE
    return serial_port_control((serial_port_handle_t) handle, (serial_port_ctrl_cmd_t) command, (serial_port_ctrl_para_t *)para);
#else
    return -1;
#endif
}

uint8_t bt_os_layer_sleep_manager_set_sleep_handle(const char *handle_name)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    return hal_sleep_manager_set_sleep_handle(handle_name);
#else
    return 0;
#endif
}

uint8_t bt_os_layer_sleep_manager_release_sleep_handle(uint8_t handle)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    return (uint8_t)(hal_sleep_manager_release_sleep_handle(handle));
#else 
    return 0;
#endif
}

uint8_t bt_os_layer_sleep_manager_lock_sleep(uint8_t handle_index)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    return (uint8_t)(hal_sleep_manager_lock_sleep(handle_index));
#else 
    return 0;
#endif
}

uint8_t bt_os_layer_sleep_manager_unlock_sleep(uint8_t handle_index)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    return (uint8_t)(hal_sleep_manager_unlock_sleep(handle_index));
#else 
    return 0;
#endif
}

bool bt_os_layer_is_isr_active(void)
{
    if( 0 == HAL_NVIC_QUERY_EXCEPTION_NUMBER) {
        return false;
    } else {
        return true;
    }
}

extern void bt_hci_log_switch(bool on_off);

void default_bt_hci_log_switch(bool on_off)
{
    return;
}

#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_hci_log_switch=_default_bt_hci_log_switch")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__ARMCC_VERSION) || defined(__CC_ARM)
#pragma weak bt_hci_log_switch = default_bt_hci_log_switch
#else
#error "Unsupported Platform"
#endif

extern bool g_bt_hci_log_enable;

void bt_os_layer_log_disable(void)
{
#if !defined (MTK_DEBUG_LEVEL_NONE)
    syslog_set_filter("*", "off", "info", false);
#endif
	//bt_hci_log_switch(false);
	return;
}

uint32_t bt_os_layer_get_hal_gpt_time(void)
{
    uint32_t count = 0;
    int32_t ret = 0;
#ifdef HAL_GPT_MODULE_ENABLED    
    ret = (int32_t)hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &count);
#endif
    if (ret < 0) {
        return 0;
    } else {
        return (count * 1000 / 32768);
    }
}

void bt_os_layer_time_consuming_to_hci_log(char* format, uint16_t len, uint32_t value)
{
    char hci_log_string[64] = {0, 0x1B, 0xFC, 60};
    if (len > 60) {
        snprintf(hci_log_string + 4, 60, "bt_os_layer_time_consuming_to_hci_log: format len error");
    } else {       
        snprintf(hci_log_string + 4, 60, format, (unsigned int)value);
    }
    bt_hci_log(0, (const void *)hci_log_string, (const uint32_t)sizeof(hci_log_string));
}
