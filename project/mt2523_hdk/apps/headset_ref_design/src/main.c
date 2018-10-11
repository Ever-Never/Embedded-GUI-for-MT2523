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
#include "hal_cache.h"
#include "hal_mpu.h"
#include "hal_uart.h"
#include "hal_clock.h"
#include "hal_clock_internal.h"
#include "hal_dvfs.h"
#include "hal_rtc.h"
#include "hal_mipi_tx_config_internal.h"
#include "hal_nvic.h"
#include "hal_dcxo.h"
#include "hal_feature_config.h"
#include "hal_audio.h"
#include "hal_sleep_manager.h"
//#include "bt_error_solution.h"

#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
#include "bt_timer_external.h"
#endif

/*bt spp include */
#include "bt_spp.h"
#include "bt_init.h"
#include "bt_notify_app.h"
#include "fota_gprot.h"

#include "nvdm.h"
#include "hal_flash.h"


#include "audio_middleware_api.h"

/* fota include */
#include "fota_gprot.h"

#include "bt_app_common.h"
#if defined(MTK_PORT_SERVICE_ENABLE)
#include "serial_port.h"
#endif

#ifdef MTK_USB_DEMO_ENABLED
#include "usb.h"
#endif

#ifdef MTK_EXTERNAL_DSP_ENABLE
#include "external_dsp_application.h"
#endif

#include "ble_ancs_app.h"
#include "sensor_alg_interface.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
extern uint8_t sleep_manager_handle;

#include "bsp_gpio_ept_config.h"
#include "memory_map.h"
//#include "hal_display_lcd.h"


#ifdef MTK_SYSTEM_CLOCK_26M
static const uint32_t target_freq = 26000;
#else
#ifdef MTK_SYSTEM_CLOCK_104M
static const uint32_t target_freq = 104000;
#else
static const uint32_t target_freq = 208000;
#endif
#endif

/* Private functions ---------------------------------------------------------*/
#if defined(MTK_CORD_FREE_HEADSET_ENABLE) || defined(MTK_INEAR_NOODLES_HEADSET_ENABLE)
    #define MTK_RTC_CLEAN_FOR_DOWNLOAD
#endif

#ifdef MTK_RTC_CLEAN_FOR_DOWNLOAD
//add james start
void rtc_wait_busy(void)
{
    uint32_t count = 0;
    while (count < 0x6EEEEE) {
        if (((*(volatile uint16_t *)0xA21e0000) & (0x40)) == 0) {
            break;
        }
        count++;
    }
}
void rtc_write_trigger_wait()
{
    *(volatile uint16_t *)0xA21e0074 = 0x1;
	rtc_wait_busy();
}
void rtc_set_rtc()
{
    uint16_t value = 0;
    *(volatile uint16_t *)0xA21e000c = 0x0;
	rtc_write_trigger_wait();
	value = *(volatile uint16_t *)0xA21e0078;
    value |= 0x100;
    *(volatile uint16_t *)0xA21e0078 = value;
	rtc_write_trigger_wait();
	
    value = *(volatile uint16_t *)0xA21e0078;
    if ((value & 0x100) == 0) {
        printf("\r\n james----------->rtc_set_rtc   fail\r\n");
    } else {
        printf("\r\n james----------->rtc_set_rtc   ok\r\n");
    }
}
void rtc_clean()
{
	uint16_t value = 0;
    value = *(volatile uint16_t *)0xA21e0078;
    value &= (~0x100);
    *(volatile uint16_t *)0xA21e0078 = value;
    rtc_write_trigger_wait();

    value = *(volatile uint16_t *)0xA21e0078;
    if ((value & 0x100) != 0) {
      //  LOG_E(RTC_ATCI, "rtc_mode clear fail\r\n");
      printf("\r\n james----------->rtc_clean   fail\r\n");
    } else {
       // LOG_E(RTC_ATCI, "rtc_mode clear ok\r\n");
        printf("\r\n james----------->rtc_clean   ok\r\n");
    }
}
//add james end
#endif

#ifdef MTK_PROMPT_SOUND_ENABLE
extern void atci_example_init();
#endif

static void SystemClock_Config(void);
static void prvSetupHardware( void );

void vApplicationTickHook(void)
{

}

int BT_XFile_EncryptionCommand()
{
    return 0;
}

#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif
{
#if (MTK_PORT_SERVICE_ENABLE)
    return log_write((char*)&ch, 1);
#else
    /* Place your implementation of fputc here */
    /* e.g. write a character to the HAL_UART_0 and Loop until the end of transmission */
    hal_uart_put_char( HAL_UART_0, ch);
    if (ch == '\n') {
        hal_uart_put_char(HAL_UART_0, '\r');
    }
    return ch;
#endif
}

static void cache_init(void)
{
    hal_cache_region_t region, region_number;

    /* Max region number is 16 */
    hal_cache_region_config_t region_cfg_tbl[] = {
        /* cacheable address, cacheable size(both MUST be 4k bytes aligned) */
#ifdef CM4_UBIN_LENGTH
        {BL_BASE, BL_LENGTH + CM4_LENGTH + CM4_UBIN_LENGTH},
#else
        {BL_BASE, BL_LENGTH + CM4_LENGTH},
#endif
        /* virtual memory */
        {VRAM_BASE, VRAM_LENGTH}
    };

    region_number = (hal_cache_region_t) (sizeof(region_cfg_tbl) / sizeof(region_cfg_tbl[0]));

    hal_cache_init();
    hal_cache_set_size(HAL_CACHE_SIZE_32KB);
    for (region = HAL_CACHE_REGION_0; region < region_number; region++) {
        hal_cache_region_config(region, &region_cfg_tbl[region]);
        hal_cache_region_enable(region);
    }
    for ( ; region < HAL_CACHE_REGION_MAX; region++) {
        hal_cache_region_disable(region);
    }
    hal_cache_enable();
}

static uint32_t caculate_mpu_region_size( uint32_t region_size )
{
    uint32_t count;

    if (region_size < 32) {
        return 0;
    }
    for (count = 0; ((region_size  & 0x80000000) == 0); count++, region_size  <<= 1);
    return 30 - count;
}

static void mpu_init(void)
{
    hal_mpu_region_t region, region_number;
    hal_mpu_region_config_t region_config;
    typedef struct {
        uint32_t mpu_region_base_address;/**< MPU region start address */
        uint32_t mpu_region_end_address;/**< MPU region end address */
        hal_mpu_access_permission_t mpu_region_access_permission; /**< MPU region access permission */
        uint8_t mpu_subregion_mask; /**< MPU sub region mask*/
        bool mpu_xn;/**< XN attribute of MPU, if set TRUE, execution of an instruction fetched from the corresponding region is not permitted */
    } mpu_region_information_t;

#if defined (__GNUC__) || defined (__CC_ARM)

    //RAM: VECTOR TABLE+RAM CODE+RO DATA
    extern uint32_t Image$$RAM_TEXT$$Base;
    extern uint32_t Image$$RAM_TEXT$$Limit;
    //TCM: TCM CODE+RO DATA
    extern uint32_t Image$$TCM$$RO$$Base;
    extern uint32_t Image$$TCM$$RO$$Limit;
    //STACK END
    extern unsigned int Image$$STACK$$ZI$$Base[];

    /* MAX region number is 8 */
    mpu_region_information_t region_information[] = {
        /* mpu_region_start_address, mpu_region_end_address, mpu_region_access_permission, mpu_subregion_mask, mpu_xn */
        {(uint32_t) &Image$$RAM_TEXT$$Base, (uint32_t) &Image$$RAM_TEXT$$Limit, HAL_MPU_READONLY, 0x0, FALSE}, //Vector table+RAM code+RAM rodata
        {(uint32_t) &Image$$RAM_TEXT$$Base + VRAM_BASE, (uint32_t) &Image$$RAM_TEXT$$Limit + VRAM_BASE, HAL_MPU_NO_ACCESS, 0x0, TRUE}, //Virtual memory
        {(uint32_t) &Image$$TCM$$RO$$Base, (uint32_t) &Image$$TCM$$RO$$Limit, HAL_MPU_READONLY, 0x0, FALSE},//TCM code+TCM rodata
        {(uint32_t) &Image$$STACK$$ZI$$Base, (uint32_t) &Image$$STACK$$ZI$$Base + 32, HAL_MPU_READONLY, 0x0, TRUE} //Stack end check for stack overflow
    };

#elif defined (__ICCARM__)

#pragma section = ".intvec"
#pragma section = ".ram_rodata"
#pragma section = ".tcm_code"
#pragma section = ".tcm_rwdata"
#pragma section = "CSTACK"

    /* MAX region number is 8, please DO NOT modify memory attribute of this structure! */
    _Pragma("location=\".ram_rodata\"") static mpu_region_information_t region_information[] = {
        /* mpu_region_start_address, mpu_region_end_address, mpu_region_access_permission, mpu_subregion_mask, mpu_xn */
        {(uint32_t)__section_begin(".intvec"), (uint32_t)__section_end(".ram_rodata"), HAL_MPU_READONLY, 0x0, FALSE},//Vector table+RAM code+RAM rodata
        {(uint32_t)__section_begin(".intvec") + VRAM_BASE, (uint32_t)__section_end(".ram_rodata") + VRAM_BASE, HAL_MPU_NO_ACCESS, 0x0, TRUE}, //Virtual memory
        {(uint32_t)__section_begin(".tcm_code"), (uint32_t)__section_begin(".tcm_rwdata"), HAL_MPU_READONLY, 0x0, FALSE},//TCM code+TCM rodata
        {(uint32_t)__section_begin("CSTACK"), (uint32_t)__section_begin("CSTACK") + 32, HAL_MPU_READONLY, 0x0, TRUE} //Stack end check for stack overflow
    };

#endif

    hal_mpu_config_t mpu_config = {
        /* PRIVDEFENA, HFNMIENA */
        TRUE, TRUE
    };

    region_number = (hal_mpu_region_t)(sizeof(region_information) / sizeof(region_information[0]));

    hal_mpu_init(&mpu_config);
    for (region = HAL_MPU_REGION_0; region < region_number; region++) {
        /* Updata region information to be configured */
        region_config.mpu_region_address = region_information[region].mpu_region_base_address;
        region_config.mpu_region_size = (hal_mpu_region_size_t) caculate_mpu_region_size(region_information[region].mpu_region_end_address - region_information[region].mpu_region_base_address);
        region_config.mpu_region_access_permission = region_information[region].mpu_region_access_permission;
        region_config.mpu_subregion_mask = region_information[region].mpu_subregion_mask;
        region_config.mpu_xn = region_information[region].mpu_xn;

        hal_mpu_region_configure(region, &region_config);
        hal_mpu_region_enable(region);
    }
    /* make sure unused regions are disabled */
    for (; region < HAL_MPU_REGION_MAX; region++) {
        hal_mpu_region_disable(region);
    }
    hal_mpu_enable();
}

static void mipi_power_init(void)
{
    MIPITX_CONFIG_REGISTER_T *mipi_tx_config_register_ptr = (MIPITX_CONFIG_REGISTER_T *)(MIPI_TX_CFG_BASE);

    /* MIPI analog module power off */
    mipi_tx_config_register_ptr->mipitx_con6_register.field.RG_LNT_BGR_EN = 0;
}

#ifdef MTK_SMART_BATTERY_ENABLE
#include "hal.h"
#include "hal_charger.h"
/* battery management includes */
#include "battery_management.h"
#include "battery_message.h"
#include "cust_battery_meter.h"
//#include "ble_bas_app.h"
log_create_module(bmt_demo, PRINT_LEVEL_WARNING);
#endif

#if !defined (MTK_DEBUG_LEVEL_NONE)

LOG_CONTROL_BLOCK_DECLARE(atci);
#ifdef MTK_BT_AT_COMMAND_ENABLE
LOG_CONTROL_BLOCK_DECLARE(atci_bt);
#endif
#ifdef MTK_CTP_ENABLE
LOG_CONTROL_BLOCK_DECLARE(atci_ctp);
#endif
LOG_CONTROL_BLOCK_DECLARE(atci_keypad);
LOG_CONTROL_BLOCK_DECLARE(atci_reg);
LOG_CONTROL_BLOCK_DECLARE(atcmd);
#ifdef MTK_SMART_BATTERY_ENABLE
LOG_CONTROL_BLOCK_DECLARE(bmt);
#endif
LOG_CONTROL_BLOCK_DECLARE(BT);
LOG_CONTROL_BLOCK_DECLARE(BTMM);
LOG_CONTROL_BLOCK_DECLARE(BTHCI);
LOG_CONTROL_BLOCK_DECLARE(BTIF);
LOG_CONTROL_BLOCK_DECLARE(BTRFCOMM);
LOG_CONTROL_BLOCK_DECLARE(BTL2CAP);
LOG_CONTROL_BLOCK_DECLARE(BTSPP);
LOG_CONTROL_BLOCK_DECLARE(NOTIFY_APP);
LOG_CONTROL_BLOCK_DECLARE(NOTIFY_SRV);
LOG_CONTROL_BLOCK_DECLARE(NOTIFY);
LOG_CONTROL_BLOCK_DECLARE(fota_dl_m);
LOG_CONTROL_BLOCK_DECLARE(sink_srv);
LOG_CONTROL_BLOCK_DECLARE(sink_music);
LOG_CONTROL_BLOCK_DECLARE(AudPly);
LOG_CONTROL_BLOCK_DECLARE(sink_app);
LOG_CONTROL_BLOCK_DECLARE(common);
LOG_CONTROL_BLOCK_DECLARE(hal);
LOG_CONTROL_BLOCK_DECLARE(sensor);
#if defined(MTK_PORT_SERVICE_ENABLE)
LOG_CONTROL_BLOCK_DECLARE(SPP_PORT);
LOG_CONTROL_BLOCK_DECLARE(bt_spp);
#endif
LOG_CONTROL_BLOCK_DECLARE(BT_DM);
LOG_CONTROL_BLOCK_DECLARE(BT_APP);
LOG_CONTROL_BLOCK_DECLARE(BT_GATTC);
LOG_CONTROL_BLOCK_DECLARE(BLE_FMPS);
LOG_CONTROL_BLOCK_DECLARE(DOGP_ADP);
LOG_CONTROL_BLOCK_DECLARE(DOGP_CM);
LOG_CONTROL_BLOCK_DECLARE(BAS_APP);
#ifdef MTK_LED_ENABLE
LOG_CONTROL_BLOCK_DECLARE(LED_APP);
#endif
LOG_CONTROL_BLOCK_DECLARE(MP3_CODEC);

log_control_block_t *syslog_control_blocks[] = {
    &LOG_CONTROL_BLOCK_SYMBOL(atci),
#ifdef MTK_BT_AT_COMMAND_ENABLE
    &LOG_CONTROL_BLOCK_SYMBOL(atci_bt),
#endif    
#ifdef MTK_CTP_ENABLE
    &LOG_CONTROL_BLOCK_SYMBOL(atci_ctp),
#endif
    &LOG_CONTROL_BLOCK_SYMBOL(atci_keypad),
    &LOG_CONTROL_BLOCK_SYMBOL(atcmd),
#ifdef MTK_SMART_BATTERY_ENABLE
    &LOG_CONTROL_BLOCK_SYMBOL(bmt),
#endif
    &LOG_CONTROL_BLOCK_SYMBOL(BT),
    &LOG_CONTROL_BLOCK_SYMBOL(BT),
    &LOG_CONTROL_BLOCK_SYMBOL(BTMM),
    &LOG_CONTROL_BLOCK_SYMBOL(BTHCI),
    &LOG_CONTROL_BLOCK_SYMBOL(BTIF),
    &LOG_CONTROL_BLOCK_SYMBOL(BTL2CAP),
    &LOG_CONTROL_BLOCK_SYMBOL(BTRFCOMM),
    &LOG_CONTROL_BLOCK_SYMBOL(BTSPP),
    &LOG_CONTROL_BLOCK_SYMBOL(sink_srv),
    &LOG_CONTROL_BLOCK_SYMBOL(sink_music),
    &LOG_CONTROL_BLOCK_SYMBOL(AudPly),
    &LOG_CONTROL_BLOCK_SYMBOL(sink_app),
    &LOG_CONTROL_BLOCK_SYMBOL(NOTIFY_APP),
    &LOG_CONTROL_BLOCK_SYMBOL(NOTIFY_SRV),
    &LOG_CONTROL_BLOCK_SYMBOL(NOTIFY),
    &LOG_CONTROL_BLOCK_SYMBOL(fota_dl_m),
    &LOG_CONTROL_BLOCK_SYMBOL(common),
    &LOG_CONTROL_BLOCK_SYMBOL(hal),
#if defined(MTK_PORT_SERVICE_ENABLE)
    &LOG_CONTROL_BLOCK_SYMBOL(SPP_PORT),
//    &LOG_CONTROL_BLOCK_SYMBOL(bt_spp),
#endif
    &LOG_CONTROL_BLOCK_SYMBOL(BT_DM),
    &LOG_CONTROL_BLOCK_SYMBOL(BT_APP),
    &LOG_CONTROL_BLOCK_SYMBOL(BT_GATTC),
    &LOG_CONTROL_BLOCK_SYMBOL(DOGP_ADP),
    &LOG_CONTROL_BLOCK_SYMBOL(DOGP_CM),
    &LOG_CONTROL_BLOCK_SYMBOL(BAS_APP),
#ifdef MTK_LED_ENABLE
    &LOG_CONTROL_BLOCK_SYMBOL(LED_APP),
#endif
    &LOG_CONTROL_BLOCK_SYMBOL(MP3_CODEC),

    NULL
};

static void syslog_config_save(const syslog_config_t *config)
{
    nvdm_status_t status;
    char *syslog_filter_buf;

    syslog_filter_buf = (char*)pvPortMalloc(SYSLOG_FILTER_LEN);
    configASSERT(syslog_filter_buf != NULL);
    syslog_convert_filter_val2str((const log_control_block_t **)config->filters, syslog_filter_buf);
    status = nvdm_write_data_item("common",
                                  "syslog_filters",
                                  NVDM_DATA_ITEM_TYPE_STRING,
                                  (const uint8_t *)syslog_filter_buf,
                                  strlen(syslog_filter_buf));
    vPortFree(syslog_filter_buf);
    LOG_I(common, "syslog config save, status=%d", status);
}

static uint32_t syslog_config_load(syslog_config_t *config)
{
    uint32_t sz = SYSLOG_FILTER_LEN;
    char *syslog_filter_buf;

    syslog_filter_buf = (char*)pvPortMalloc(SYSLOG_FILTER_LEN);
    configASSERT(syslog_filter_buf != NULL);
    if (nvdm_read_data_item("common", "syslog_filters", (uint8_t*)syslog_filter_buf, &sz) == NVDM_STATUS_OK) {
        syslog_convert_filter_str2val(config->filters, syslog_filter_buf);
    } else {
        /* popuplate the syslog nvdm with the image setting */
        syslog_config_save(config);
    }
    vPortFree(syslog_filter_buf);
    return 0;
}

#endif /* MTK_DEBUG_LEVEL_NONE */

#ifdef MTK_SMART_BATTERY_ENABLE
/**
* @brief       This function is use to get the battery information.
* @return      None.
*/
extern void bt_sink_app_battery_report_handler(int32_t charger_exist, uint8_t capacity);
static void get_battery_information(void)
{
    int32_t capacity, charger_current, charger_status, charger_type, battery_temperature, battery_volt, capacity_level;

    capacity = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);
    capacity_level = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY_LEVEL);
    charger_current = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGING_CURRENT);
    charger_status = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST);
    charger_type = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_TYPE);
    battery_temperature = battery_management_get_battery_property(BATTERY_PROPERTY_TEMPERATURE);
    battery_volt = battery_management_get_battery_property(BATTERY_PROPERTY_VOLTAGE);

    if (charger_status) {
        LOG_I(bmt_demo, "Battery capacity = %d(%%)", capacity);
        LOG_I(bmt_demo, "Battery capacity level = %d", capacity_level);
        LOG_I(bmt_demo, "Charger current = %d(mA)", charger_current);
        LOG_I(bmt_demo, "Charger status = %d(0:charge not exist; 1:charger exist)", charger_status);
        LOG_I(bmt_demo, "Charger type = %d(0:unknown; 1:standard host; 2:non-standard AC charger; 3:standard AC charger 4:charging host)", charger_type);
        LOG_I(bmt_demo, "Battery temperature = %d(Celsius degrees)", battery_temperature);
        LOG_I(bmt_demo, "Battery voltage = %d(mV)\n", battery_volt);
    } else {
        LOG_I(bmt_demo, "Battery capacity = %d(%%)", capacity);
        LOG_I(bmt_demo, "Battery capacity level = %d", capacity_level);
        LOG_I(bmt_demo, "Battery temperature = %d(Celsius degrees)", battery_temperature);
        LOG_I(bmt_demo, "Battery voltage = %d(mV)\n", battery_volt);
    }
    //ble_bas_app_update_battery_capacity(capacity);
    bt_sink_app_battery_report_handler(charger_status, capacity);
}

/**
* @brief       This function is use to check battery status.
* @return      None.
*/
static void check_battery_status(void)
{
    int32_t capacity, battery_temperature,charger_exist;

    capacity = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);
    battery_temperature = battery_management_get_battery_property(BATTERY_PROPERTY_TEMPERATURE);
    charger_exist = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST);

    if((capacity == 0 ) && (capacity != BATTERY_INVALID_VALUE)) {
        /* Low battery shutdown */
        LOG_W(bmt_demo, "Low battery shutdown");
        vTaskDelay(3 * 1000 / portTICK_RATE_MS);
        hal_sleep_manager_enter_power_off_mode();
    }

    /* High temperature protection. It depends on your application and the battery specifications */
    if((battery_temperature >= 60 ) && (battery_temperature != BATTERY_INVALID_VALUE)) {
        /* High temperature  shutdown */
        LOG_W(bmt_demo, "High temperature shutdown");
        vTaskDelay(3 * 1000 / portTICK_RATE_MS);
        hal_sleep_manager_enter_power_off_mode();
    }

    if((charger_exist == 1 ) && (charger_exist != BATTERY_INVALID_VALUE)
       && (capacity == 100 ) && (capacity != BATTERY_INVALID_VALUE)) {
        /* Battery full */
        LOG_I(bmt_demo, "Battery full");
    }

}

/**
* @brief       This function is battery management message handler.
* @param[in]   message: The message should be process.
* @return      None.
*/
static void battery_management_message_handler(battery_message_context_t *message)
{
    LOG_I(bmt_demo, "battery_management_message_handler event  = 0x%X", (int) message->event);

    switch (message->event) {
        case BATTERY_MANAGEMENT_EVENT_BATTERY_UPDATE: {

            get_battery_information();

        }
        break;
        default: {

        }
        break;
    }

    check_battery_status();
}

/**
* @brief       Task main function
* @param[in]   pvParameters: Pointer that will be used as the parameter for the task being created.
* @return      None.
*/
static void battery_message_task(void *pvParameters)
{
    uint32_t handle;
    battery_message_context_t message;

    battery_message_allocate(&handle);
	
	#ifdef MTK_RTC_CLEAN_FOR_DOWNLOAD
	//add james start
	rtc_set_rtc();
	rtc_clean();
	 //add james end
	#endif

    while (1) {
        if (battery_message_receive(handle, &message)) {

            battery_management_message_handler(&message);
        }
    }
}

/**
 * @brief     This function is to create task for battery message.
 * @return    None.
 */
void battery_message_create_task(void)
{
    /* Create task */
    xTaskCreate(battery_message_task, BMT_APP_TASK_NAME, BMT_APP_TASK_STACKSIZE / sizeof(StackType_t), NULL, BMT_APP_TASK_PRIO, NULL);
}
#endif //MTK_SMART_BATTERY_ENABLE


#if defined(MTK_PORT_SERVICE_ENABLE)
static void syslog_port_service_init(void)
{
    serial_port_dev_t syslog_port;
    serial_port_setting_uart_t uart_setting;

    if (serial_port_config_read_dev_number("syslog", &syslog_port) != SERIAL_PORT_STATUS_OK) {
        syslog_port = SERIAL_PORT_DEV_UART_0;
        serial_port_config_write_dev_number("syslog", syslog_port);
        uart_setting.baudrate = HAL_UART_BAUDRATE_115200;
        serial_port_config_write_dev_setting(syslog_port, (serial_port_dev_setting_t *)&uart_setting);
    }
}
#endif

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    /* SystemClock Config */
    SystemClock_Config();

    SystemCoreClockUpdate();

    /* init UART earlier */
    log_uart_init(HAL_UART_0);

    /* Configure the hardware ready to run the test. */
    prvSetupHardware();

    hal_sleep_manager_init();

#ifdef MTK_SMART_BATTERY_ENABLE
    battery_management_init();

    battery_message_create_task();
#endif

    clock_dump_info();

    nvdm_init();

    log_init(syslog_config_save, syslog_config_load, syslog_control_blocks);
    
    log_config_print_switch(BTMM, DEBUG_LOG_OFF);

#ifdef MTK_USB_DEMO_ENABLED
    usb_boot_init();
#endif

#if defined(MTK_PORT_SERVICE_ENABLE)
    syslog_port_service_init();
#endif
    audio_middleware_init();

    hal_audio_init();

    bt_demo_init();

    task_def_init();

#if defined(__AFE_HS_DC_CALIBRATION__)
    // Currently, only HS DC calibration data would be saved to nvdm. Called after hal_audio_init(@task_def_init).
    audio_nvdm_save_dc_data_to_nvdm();
#endif

    task_def_create();

#ifdef MTK_PROMPT_SOUND_ENABLE
    atci_example_init();
#endif

    sensor_manager_init();
    bt_app_common_init();
    bt_notify_app_init();

    ble_ancs_app_init();
    /* fota demo */
    fota_download_manager_init();

#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    /* sink timer init */
    bt_timer_ext_init();
#endif

    /* Call this function to indicate the system initialize done. */
    SysInitStatus_Set();

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for ( ;; );
}

static void prvSetupHardware( void )
{
    /* system HW init */
    cache_init();
    mpu_init();

    /* peripherals init */
    hal_flash_init();

    hal_nvic_init();

    bsp_ept_gpio_setting_init();
    
    hal_rtc_init();
    hal_dcxo_init();

    mipi_power_init();
}

static void SystemClock_Config(void)
{
    hal_clock_init();

//    hal_display_lcd_set_clock(HAL_DISPLAY_LCD_INTERFACE_CLOCK_124MHZ);

    pmu_init();

    hal_dvfs_init();
    hal_dvfs_target_cpu_frequency(target_freq, HAL_DVFS_FREQ_RELATION_H);
}

