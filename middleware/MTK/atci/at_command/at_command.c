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

// System head file
#include "FreeRTOS.h"
#include "task.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// For Register AT command handler
#include "at_command.h"
#include "hal_feature_config.h"

#include "verno.h"
#include "syslog.h"

log_create_module(atcmd, PRINT_LEVEL_WARNING);

#define LOGE(fmt,arg...)   LOG_E(atcmd, "ATCMD: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atcmd, "ATCMD: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atcmd ,"ATCMD: "fmt,##arg)

/*--- Function ---*/
atci_status_t atci_cmd_hdlr_test(atci_parse_cmd_param_t *parse_cmd);
#if defined(MTK_QUERY_SDK_VERSION) && defined(MTK_FW_VERSION)
extern atci_status_t atci_cmd_hdlr_sdkinfo(atci_parse_cmd_param_t *parse_cmd);
#endif //  defined(MTK_QUERY_SDK_VERSION) && defined(MTK_FW_VERSION)

#ifndef MTK_AT_CMD_DISABLE
/*--- Function ---*/
#if defined(__GNUC__)
#ifdef TOOL_APP_MODULE
extern atci_status_t atci_cmd_hdlr_testframework(atci_parse_cmd_param_t *parse_cmd);
#endif
#endif

extern atci_status_t atci_cmd_hdlr_syslog(atci_parse_cmd_param_t *parse_cmd);
extern atci_status_t atci_cmd_hdlr_mem(atci_parse_cmd_param_t *parse_cmd);

#if defined(DEVICE_HDK) || defined(DEVICE_BAND)

#ifdef MTK_SENSOR_BIO_USE
extern atci_status_t atci_cmd_hdlr_biogui(atci_parse_cmd_param_t *parse_cmd);
#endif

#else // defined(DEVICE_HDK) || defined(DEVICE_BAND)
extern atci_status_t atci_cmd_hdlr_camera(atci_parse_cmd_param_t *parse_cmd);

#ifdef MTK_GNSS_ENABLE
extern atci_status_t gnss_power_control_at_handler(atci_parse_cmd_param_t *parse_cmd);
extern atci_status_t gnss_send_command_at_handler(atci_parse_cmd_param_t *parse_cmd);
#endif

#ifdef HAL_DISPLAY_LCD_MODULE_ENABLED
extern	atci_status_t atci_cmd_hdlr_lcm(atci_parse_cmd_param_t *parse_cmd);
#endif

#ifdef HAL_CHARGER_MODULE_ENABLED
extern atci_status_t atci_cmd_hdlr_charger(atci_parse_cmd_param_t *parse_cmd);
#endif

#ifdef MTK_EXTERNAL_GAUGE
#ifdef MTK_BUILD_SMT_LOAD
extern atci_status_t atci_cmd_hdlr_external_gauge(atci_parse_cmd_param_t *parse_cmd);
#endif
#endif

#ifndef MTK_EXTERNAL_PMIC
extern atci_status_t atci_cmd_hdlr_pmu(atci_parse_cmd_param_t *parse_cmd);
#endif

#ifdef MTK_EXTERNAL_PMIC
extern atci_status_t atci_cmd_hdlr_external_pmic(atci_parse_cmd_param_t *parse_cmd);
#endif

#ifdef HAL_REG_MODULE_ENABLED
extern atci_status_t atci_cmd_hdlr_reg(atci_parse_cmd_param_t *parse_cmd);
#endif

#ifdef HAL_RTC_MODULE_ENABLED
extern atci_status_t atci_cmd_hdlr_rtc(atci_parse_cmd_param_t *parse_cmd);
#endif

#if defined(HAL_SD_MODULE_ENABLED)
#if defined(MTK_BUILD_SMT_LOAD) || defined(MTK_BUILD_HQA_LOAD)
extern atci_status_t atci_cmd_hdlr_msdc(atci_parse_cmd_param_t *parse_cmd);
#endif
#endif

#if defined(MTK_NVDM_ENABLE)
extern atci_status_t atci_cmd_hdlr_nvdm(atci_parse_cmd_param_t *parse_cmd);
#endif

#if defined(MTK_PORT_SERVICE_ENABLE)
extern atci_status_t atci_cmd_hdlr_serial_port(atci_parse_cmd_param_t *parse_cmd);
#endif

#if defined(HAL_GPIO_MODULE_ENABLED)
#ifdef MTK_BUILD_SMT_LOAD
extern atci_status_t atci_cmd_hdlr_led(atci_parse_cmd_param_t *parse_cmd);
#endif
extern atci_status_t atci_cmd_hdlr_gpio(atci_parse_cmd_param_t *parse_cmd);
#endif

#if defined(__GNUC__)
#ifdef HAL_AES_MODULE_ENABLED
extern atci_status_t atci_cmd_hdlr_crypto(atci_parse_cmd_param_t *parse_cmd);
#endif
#endif

#ifdef HAL_DVFS_MODULE_ENABLED
extern atci_status_t atci_cmd_hdlr_dvfs_get(atci_parse_cmd_param_t *parse_cmd);
extern atci_status_t atci_cmd_hdlr_dvfs_set(atci_parse_cmd_param_t *parse_cmd);
extern atci_status_t atci_cmd_hdlr_dvfs_dbg(atci_parse_cmd_param_t *parse_cmd);
#endif

#ifdef HAL_CLOCK_MODULE_ENABLED
extern atci_status_t atci_cmd_hdlr_clock(atci_parse_cmd_param_t *parse_cmd);
#endif

#if defined(HAL_DISPLAY_PWM_MODULE_ENABLED) || defined(HAL_ISINK_MODULE_ENABLED)
extern atci_status_t atci_cmd_hdlr_backlight(atci_parse_cmd_param_t *parse_cmd);
#endif

#if (PRODUCT_VERSION == 2523 || PRODUCT_VERSION == 2533 )
#ifdef HAL_SLEEP_MANAGER_ENABLED
extern atci_status_t atci_cmd_hdlr_sleep_manager(atci_parse_cmd_param_t *parse_cmd);
#endif
#endif

#if defined(__GNUC__)
#if defined(HAL_AUDIO_MODULE_ENABLED)
extern atci_status_t atci_cmd_hdlr_audio(atci_parse_cmd_param_t *parse_cmd);
#endif
#endif

#if defined(MTK_AUDIO_TUNING_ENABLED)
extern atci_status_t atci_cmd_hdlr_eaps(atci_parse_cmd_param_t *parse_cmd);
extern atci_status_t atci_cmd_hdlr_attdet(atci_parse_cmd_param_t *parse_cmd);
extern atci_status_t atci_cmd_hdlr_atttest(atci_parse_cmd_param_t *parse_cmd);
#endif

#ifdef __CFW_CONFIG_MODE__
extern atci_status_t atci_cmd_hdlr_cfw(atci_parse_cmd_param_t *parse_cmd);
#endif
#if defined(MTK_BT_AT_COMMAND_ENABLE)
extern atci_status_t atci_cmd_hdlr_bt_ata(atci_parse_cmd_param_t *parse_cmd);
extern atci_status_t atci_cmd_hdlr_bt_power(atci_parse_cmd_param_t *parse_cmd);
extern atci_status_t atci_cmd_hdlr_bt_relay(atci_parse_cmd_param_t *parse_cmd);
extern atci_status_t atci_cmd_hdlr_bt_send_hci_command(atci_parse_cmd_param_t *parse_cmd);
extern atci_status_t atci_cmd_hdlr_bt_testbox(atci_parse_cmd_param_t *parse_cmd);
extern atci_status_t atci_cmd_hdlr_capid(atci_parse_cmd_param_t *parse_cmd);
#endif

#ifdef __BT_DEBUG__
    extern atci_status_t atci_cmd_hdlr_bt_enable_driver_dump_log(atci_parse_cmd_param_t *parse_cmd);
#endif

#ifndef MTK_EXTERNAL_PMIC
extern atci_status_t atci_cmd_hdlr_vibrator(atci_parse_cmd_param_t *parse_cmd);
#endif


#ifdef MTK_CTP_ENABLE
extern atci_status_t atci_cmd_hdlr_ctp(atci_parse_cmd_param_t *parse_cmd);
extern atci_status_t atci_cmd_hdlr_ctp_simulate(atci_parse_cmd_param_t *parse_cmd);
#endif

#ifdef HAL_KEYPAD_MODULE_ENABLED
atci_status_t atci_cmd_hdlr_keypad(atci_parse_cmd_param_t *parse_cmd);
extern atci_status_t atci_cmd_hdlr_keypad_simulate(atci_parse_cmd_param_t *parse_cmd);
#endif

#ifdef HAL_WDT_MODULE_ENABLED
atci_status_t atci_cmd_hdlr_wdt(atci_parse_cmd_param_t *parse_cmd);
#endif


#ifdef MTK_SENSOR_AT_COMMAND_ENABLE
extern  atci_status_t atci_cmd_hdlr_sensor(atci_parse_cmd_param_t *parse_cmd);
#endif

#ifdef MTK_SENSOR_ACCELEROMETER_USE_BMA255
extern atci_status_t atci_cmd_hdlr_gsensor(atci_parse_cmd_param_t *parse_cmd);
#endif

#ifdef MTK_SENSOR_BIO_USE
extern atci_status_t atci_cmd_hdlr_biogui(atci_parse_cmd_param_t *parse_cmd);
#endif

#endif //end of DEVICE_HDK || DEVICE_BAND

#ifdef MTK_SYSTEM_AT_COMMAND_ENABLE
extern atci_status_t atci_cmd_hdlr_system(atci_parse_cmd_param_t *parse_cmd);
#endif

#ifdef MTK_OS_CPU_UTILIZATION_ENABLE
extern atci_status_t atci_cmd_hdlr_utilization(atci_parse_cmd_param_t *parse_cmd);
#endif

#if defined(MTK_FOTA_ENABLE) && defined(MTK_FOTA_AT_COMMAND_ENABLE)
extern atci_status_t atci_cmd_hdlr_fota_cmd(atci_parse_cmd_param_t *parse_cmd);
#endif

#if defined(MTK_FW_VERSION) && ((PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697) || (PRODUCT_VERSION == 7686) || (PRODUCT_VERSION == 7682) || (PRODUCT_VERSION == 5932))
extern atci_status_t atci_cmd_hdlr_sys(atci_parse_cmd_param_t *parse_cmd);
#endif // defined(MTK_FW_VERSION) && ((PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697) || (PRODUCT_VERSION == 7686) )	

#endif // MTK_AT_CMD_DISABLE
/*---  Variant ---*/


/* the comands in atcmd_table are supported when atci feature is on.
put all the default at cmd handler into atcmd_table, then they will be  registed in atci_init() */

atci_cmd_hdlr_item_t atcmd_table[] = {

#ifndef MTK_AT_CMD_DISABLE

#if defined(DEVICE_HDK) || defined(DEVICE_BAND)
#ifdef MTK_SENSOR_BIO_USE
    {"AT+EBIO", atci_cmd_hdlr_biogui, 0, 0},
#endif

#else //else defined(DEVICE_HDK) || defined(DEVICE_BAND)

#ifdef MTK_ATCI_CAMERA_ENABLE
    {"AT+ECMP",        atci_cmd_hdlr_camera,       0, 0},
#endif


#ifdef HAL_DISPLAY_LCD_MODULE_ENABLED
    {"AT+ELCM",        atci_cmd_hdlr_lcm,       0, 0},
#endif
#ifdef MTK_GNSS_ENABLE
    {"AT+EGPSC", gnss_power_control_at_handler, 0, 0},
    {"AT+EGPSS", gnss_send_command_at_handler , 0, 0},
#endif

#ifdef HAL_CHARGER_MODULE_ENABLED
    {"AT+ECHAR", atci_cmd_hdlr_charger, 0, 0},
#endif

#ifdef MTK_EXTERNAL_GAUGE
#ifdef MTK_BUILD_SMT_LOAD
    {"AT+EEXTGAUGE", atci_cmd_hdlr_external_gauge, 0, 0},
#endif
#endif

#ifndef MTK_EXTERNAL_PMIC
    {"AT+EPMUREG",	atci_cmd_hdlr_pmu,	    0, 0},
#endif

#ifdef MTK_EXTERNAL_PMIC
	{"AT+EPMICREG",	atci_cmd_hdlr_external_pmic,	    0, 0},
#endif

#ifdef HAL_REG_MODULE_ENABLED
    {"AT+EREG", atci_cmd_hdlr_reg,          0, 0},
#endif

#ifdef HAL_RTC_MODULE_ENABLED
    {"AT+ERTCREG",	atci_cmd_hdlr_rtc,		0, 0},
    {"AT+ERTC",	atci_cmd_hdlr_rtc,		0, 0},
    {"AT+ERTC32K",	atci_cmd_hdlr_rtc,		0, 0},
#endif

#if defined(HAL_SD_MODULE_ENABLED)
#if defined(MTK_BUILD_SMT_LOAD) || defined(MTK_BUILD_HQA_LOAD)
    {"AT+EMSDC",	atci_cmd_hdlr_msdc,	    0, 0},
#endif
#endif

#if defined(MTK_NVDM_ENABLE)
    {"AT+ENVDM", atci_cmd_hdlr_nvdm, 0, 0},
#endif

#if defined(MTK_PORT_SERVICE_ENABLE)
    {"AT+EPORT", atci_cmd_hdlr_serial_port, 0, 0},
#endif

#if defined(HAL_GPIO_MODULE_ENABLED)
#ifdef MTK_BUILD_SMT_LOAD
    {"AT+ELED", atci_cmd_hdlr_led, 0, 0},
#endif
    {"AT+EGPIO", atci_cmd_hdlr_gpio, 0, 0},
#endif

#if defined(HAL_DISPLAY_PWM_MODULE_ENABLED) || defined(HAL_ISINK_MODULE_ENABLED)
    {"AT+EBLT", atci_cmd_hdlr_backlight, 0, 0},
#endif
#if (PRODUCT_VERSION == 2523 || PRODUCT_VERSION == 2533 )
#ifdef HAL_SLEEP_MANAGER_ENABLED
    {"AT+SM", atci_cmd_hdlr_sleep_manager, 0, 0},
#endif
#endif

#if defined(__GNUC__)
#if defined(HAL_AUDIO_MODULE_ENABLED)
    {"AT+EAUDIO", atci_cmd_hdlr_audio, 0, 0},
#endif
#endif

#ifndef MTK_EXTERNAL_PMIC
    {"AT+EVIB", atci_cmd_hdlr_vibrator, 0, 0},
#endif

#if defined(MTK_AUDIO_TUNING_ENABLED)
    {"AT+EAPS", atci_cmd_hdlr_eaps, 0, 0},
    {"AT+ATTDET", atci_cmd_hdlr_attdet, 0, 0},
    {"AT+ATTTEST", atci_cmd_hdlr_atttest, 0, 0},
#endif

#ifdef MTK_CTP_ENABLE
    {"AT+EPENURC", atci_cmd_hdlr_ctp, 0, 0},
    {"AT+ECTP", atci_cmd_hdlr_ctp_simulate, 0, 0},
#endif

#ifdef HAL_KEYPAD_MODULE_ENABLED
    {"AT+EKEYURC", atci_cmd_hdlr_keypad, 0, 0},
    {"AT+EKP", atci_cmd_hdlr_keypad_simulate, 0, 0},
#endif

#ifdef MTK_SENSOR_AT_COMMAND_ENABLE
    {"AT+ESENSOR", atci_cmd_hdlr_sensor, 0, 0},
    {"AT+ESENSORTEST", atci_cmd_hdlr_sensor, 0, 0},
#endif

#ifdef __CFW_CONFIG_MODE__
    {"AT+ECFG", atci_cmd_hdlr_cfw, 0, 0},
#endif
#ifdef MTK_BT_AT_COMMAND_ENABLE
    {"AT+EBTAT", atci_cmd_hdlr_bt_ata, 0, 0},
    {"AT+EBTPW", atci_cmd_hdlr_bt_power, 0, 0},
    {"AT+EBTER", atci_cmd_hdlr_bt_relay, 0, 0},
    {"AT+EBTSHC", atci_cmd_hdlr_bt_send_hci_command, 0, 0},
    {"AT+EBTTB", atci_cmd_hdlr_bt_testbox, 0, 0},
    {"AT+CAPID", atci_cmd_hdlr_capid, 0, 0},
#endif

#ifdef __BT_DEBUG__
    {"AT+EBTEDL", atci_cmd_hdlr_bt_enable_driver_dump_log, 0, 0},
#endif

#ifdef HAL_WDT_MODULE_ENABLED
    {"AT+EWDT", atci_cmd_hdlr_wdt, 0, 0},
#endif

#if defined(__GNUC__)
#ifdef HAL_AES_MODULE_ENABLED
    {"AT+ECRYPTO", atci_cmd_hdlr_crypto, 0, 0},
#endif
#endif

#ifdef HAL_DVFS_MODULE_ENABLED
    {"AT+CPUFGET", atci_cmd_hdlr_dvfs_get, 0, 0},
    {"AT+CPUFSET", atci_cmd_hdlr_dvfs_set, 0, 0},
    {"AT+DVFSDBG", atci_cmd_hdlr_dvfs_dbg, 0, 0},
#endif

#ifdef HAL_CLOCK_MODULE_ENABLED
    {"AT+CLOCK",       atci_cmd_hdlr_clock,      0, 0},
#endif //HAL_CLOCK_MODULE_ENABLED

#if defined(__GNUC__)
#ifdef TOOL_APP_MODULE
    {"AT+TF",atci_cmd_hdlr_testframework,0, 0},
#endif
#endif

#if !defined(MTK_DEBUG_LEVEL_NONE)
    {"AT+SYSLOG",   atci_cmd_hdlr_syslog, 0, 0},
#endif

#ifdef MTK_SENSOR_ACCELEROMETER_USE_BMA255
    {"AT+EGSENSOR", atci_cmd_hdlr_gsensor, 0, 0},
#endif

#ifdef MTK_SENSOR_BIO_USE
    {"AT+EBIO", atci_cmd_hdlr_biogui, 0, 0},
#endif

#endif //end of defined(DEVICE_HDK) || defined(DEVICE_BAND)

#if defined(MTK_FOTA_ENABLE) && defined(MTK_FOTA_AT_COMMAND_ENABLE)
    {"AT+FOTA", atci_cmd_hdlr_fota_cmd, 0, 0},
#endif

#ifdef MTK_OS_CPU_UTILIZATION_ENABLE
    {"AT+UTILIZATION", atci_cmd_hdlr_utilization, 0, 0},
#endif
#ifdef MTK_SYSTEM_AT_COMMAND_ENABLE
    {"AT+SYSTEM", atci_cmd_hdlr_system, 0, 0},
#endif

    {"AT+MEM",          atci_cmd_hdlr_mem,          0, 0},

#if defined(MTK_FW_VERSION) && ((PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697) || (PRODUCT_VERSION == 7686) || (PRODUCT_VERSION == 7682) || (PRODUCT_VERSION == 5932))	
	{"AT+SYS", 		atci_cmd_hdlr_sys, 		0, 0},
#endif // defined(MTK_FW_VERSION) && ((PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697) || (PRODUCT_VERSION == 7686) )
#endif // MTK_AT_CMD_DISABLE

#if defined(MTK_QUERY_SDK_VERSION) && defined(MTK_FW_VERSION)
    {"AT+EVERINFO",       atci_cmd_hdlr_sdkinfo,      0, 0},
#endif //  defined(MTK_QUERY_SDK_VERSION) && defined(MTK_FW_VERSION)	

    {"AT+TEST",         atci_cmd_hdlr_test,         0, 0}
};


atci_status_t at_command_init(void)
{
    atci_status_t ret = ATCI_STATUS_REGISTRATION_FAILURE;
    //int32_t item_size;

    /* -------  Scenario: register AT handler in CM4 -------  */
    ret = atci_register_handler(atcmd_table, sizeof(atcmd_table) / sizeof(atci_cmd_hdlr_item_t));
    if (ret == ATCI_STATUS_OK) {
        LOGI("at_cmd_init register success\r\n");
    } else {
        LOGE("at_cmd_init register fail\r\n");
    }

    return ret;
}

// AT command handler
atci_status_t atci_cmd_hdlr_test(atci_parse_cmd_param_t *parse_cmd)
{
    int read_value = 0;
    atci_response_t response = {{0}};
    char *param = NULL;
    int  param1_val = -1;

    LOGI("atci_cmd_hdlr_test\r\n");

    response.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+TEST=?
            LOGI("AT Test OK.\n");
            strcpy((char *)response.response_buf, "+TEST:(0,1,2,3)\r\nOK\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+TEST?
            LOGI("AT Read done.\n");
            snprintf((char *)response.response_buf, sizeof(response.response_buf), "+TEST:%d\r\n", read_value);
            response.response_len = strlen((char *)response.response_buf);
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&response);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+TEST
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
			strcpy((char *)response.response_buf, "ERROR\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+TEST=<p1>  the handler need to parse the parameters
            LOGI("AT Executing...\r\n");
            //parsing the parameter
            param = strtok(parse_cmd->string_ptr, ",\n\r");
            param = strtok(parse_cmd->string_ptr, "AT+TEST=");

	    if (param != NULL)
                param1_val = atoi(param);

            if (param != NULL && (param1_val == 0 || param1_val == 1)) {

                // valid parameter, update the data and return "OK"
                response.response_len = 0;
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK; // ATCI will help append "OK" at the end of response buffer
                atci_send_response(&response);
            } else if (param != NULL && (param1_val == 2 || param1_val == 3)) {
                if (param1_val == 2) {
					log_config_print_level(atci, PRINT_LEVEL_INFO);
					log_config_print_level(atcmd, PRINT_LEVEL_INFO);
					LOGI("change atci log print level into info.\n");
					strcpy((char *)response.response_buf, "PRINT_LEVEL_INFO\r\nOK\r\n");
                    response.response_len = strlen((char *)response.response_buf);
                    atci_send_response(&response);
                } else {
                    log_config_print_level(atci, PRINT_LEVEL_WARNING);
					log_config_print_level(atcmd, PRINT_LEVEL_WARNING);
					LOGW("change atci log print level into warning.\n");
					strcpy((char *)response.response_buf, "PRINT_LEVEL_WARNING\r\nOK\r\n");
                    response.response_len = strlen((char *)response.response_buf);
                    atci_send_response(&response);
                }
				break;
			} else {
                // invalide parameter, return "ERROR"
                response.response_len = 0;
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of response buffer
                atci_send_response(&response);
            };

			for(int i = 0; i < (ATCI_UART_TX_FIFO_BUFFER_SIZE - 10); i++)
				response.response_buf[i] = '0' + i%10;
			response.response_len = ATCI_UART_TX_FIFO_BUFFER_SIZE - 10;
			response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
			response.response_flag |= ATCI_RESPONSE_FLAG_URC_FORMAT;
            atci_send_response(&response);
            param = NULL;
            break;
        default :
            strcpy((char *)response.response_buf, "ERROR\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
    return ATCI_STATUS_OK;
}

