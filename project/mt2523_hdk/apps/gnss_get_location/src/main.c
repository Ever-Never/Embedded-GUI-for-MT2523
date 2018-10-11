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

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "task_def.h"

/* device.h includes */
#include "mt2523.h"

/* hal includes */
#include "hal.h"

#include "syslog.h"
#include "sys_init.h"

/* application includes */
#include "bt_init.h"
#include "gnss_app.h"
#include "nvdm.h"
#include "ble_app_common.h"

#include "atci.h"

#if defined(MTK_PORT_SERVICE_ENABLE)
#include "serial_port.h"
#endif

#ifdef MTK_USB_DEMO_ENABLED
#include "usb.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void atcmd_serial_port_init(void);
#ifdef __cplusplus
}
#endif
static void atci_def_task(void *param)
{
    while (1) {
        atci_processing();
    }
}

#if defined(MTK_PORT_SERVICE_ENABLE)
static void syslog_port_service_init(void)
{
    serial_port_dev_t syslog_port;
    serial_port_setting_uart_t uart_setting;

    if (serial_port_config_read_dev_number((char *)"syslog", &syslog_port) != SERIAL_PORT_STATUS_OK) {
        syslog_port = SERIAL_PORT_DEV_USB_COM2;
        serial_port_config_write_dev_number((char *)"syslog", syslog_port);
        uart_setting.baudrate = HAL_UART_BAUDRATE_115200;
        serial_port_config_write_dev_setting(syslog_port, (serial_port_dev_setting_t *)&uart_setting);
    }
}
#endif

/* Create the log control block as user wishes. Here we use 'template' as module name.
* User needs to define their own log control blocks as project needs.
* Please refer to the log dev guide under /doc folder for more details.
*/

/**
* @brief       Main function
* @param[in]   None.
* @return      None.
*/
int main(void)
{
    TaskHandle_t task_handler;
    /* Do system initialization, eg: hardware, nvdm. */
    system_init();
    
    /* system log initialization.
    * This is the simplest way to initialize system log, that just inputs three NULLs
    * as input arguments. User can use advanced feature of system log along with NVDM.
    * For more details, please refer to the log dev guide under /doc folder or projects
    * under project/mtxxxx_hdk/apps/.
    */
    log_init(NULL, NULL, NULL);


    nvdm_init();
    
    /* Here, the infrastructure is setup and user can initialize function hardware like:
    * GPS, audio, LCD etc.
    * User can call initialization APIs below here or in user task at runtime.
    * EX:  gnss_init();
    *      hal_audio_init();
    *      bsp_lcd_init();
    *      hal_g2d_init();
    */
    
    /* As for generic HAL init APIs like: hal_uart_init(), hal_gpio_init() and hal_spi_master_init() etc,
    * user can call them when they need, which means user can call them here or in user task at runtime.
    */
    
    /* user needs to create BT task to do BT initialization,
    since BT initialization has to be done in task level.
    */
    bt_create_task();
    ble_app_common_init();
    /* user may create own tasks here.
    * EX: 
    *    xTaskCreate(UserTask, USER_TASK_NAME, USER_TASK_STACKSIZE/sizeof(StackType_t), NULL, USER_TASK_PRIO, NULL);
    *
    *    UserTask is user's task entry function, which may be defined in another C file to do application job.
    *    USER_TASK_NAME, USER_TASK_STACKSIZE and USER_TASK_PRIO should be defined in task_def.h. User needs to
    *    refer to example in task_def.h, then makes own task MACROs defined.
    */
#ifdef MTK_USB_DEMO_ENABLED
            usb_boot_init();
#endif
        
#if defined(MTK_PORT_SERVICE_ENABLE)
            syslog_port_service_init();
#endif
        
#if defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)
        {
            serial_port_dev_t port;
            serial_port_setting_uart_t uart_setting;
        
            if(serial_port_config_read_dev_number((char *)"atci", &port) != SERIAL_PORT_STATUS_OK)
            {
                port = SERIAL_PORT_DEV_USB_COM1; //SERIAL_PORT_DEV_USB_COM1;//SERIAL_PORT_DEV_UART_1;
                serial_port_config_write_dev_number("atci", port);
                LOG_W(common, "serial_port_config_write_dev_number setting uart1");
                uart_setting.baudrate = HAL_UART_BAUDRATE_115200;
                serial_port_config_write_dev_setting(port, (serial_port_dev_setting_t *)&uart_setting);
            }
        
            atci_init(port);
        
        }
#else
            atci_init(HAL_UART_1);
#endif

    xTaskCreate(atci_def_task,
                 ATCI_TASK_NAME,
                 ATCI_TASK_STACKSIZE/(( uint32_t )sizeof( StackType_t )),
                 NULL,
                 ATCI_TASK_PRIO,
                 NULL);
    
    gnss_demo_app_create();
    xTaskCreate(epo_download_task, 
        GNSS_EPO_TASK_NAME, 
        GNSS_EPO_TASK_STACK_SIZE/(( uint32_t )sizeof( StackType_t )), 
        NULL, 
        GNSS_EPO_TASK_PRIO, 
        &task_handler );

    /* Call this function to indicate the system initialize done. */
    SysInitStatus_Set();

    /* Start the scheduler. */
    vTaskStartScheduler();
    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for( ;; );
}


