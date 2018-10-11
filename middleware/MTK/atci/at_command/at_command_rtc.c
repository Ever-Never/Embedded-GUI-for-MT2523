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

// For Register AT command handler
// System head file
#include "hal_feature_config.h"
#ifdef HAL_RTC_MODULE_ENABLED
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "at_command.h"
#include "hal_rtc.h"
#include "hal_gpt.h"
#include "hal_wdt.h"
#include "syslog.h"
#include <ctype.h>

log_create_module(RTC_ATCI, PRINT_LEVEL_INFO);
/*
 * sample code
*/

/*--- Function ---*/
extern void rtc_set_register(uint16_t address, uint16_t value);
extern uint16_t rtc_get_register(uint16_t address);
extern uint32_t f32k_measure_clock(uint16_t fixed_clock, uint16_t tested_clock, uint16_t window_setting);
extern uint16_t rtc_read_osc32con(void);
atci_status_t atci_cmd_hdlr_rtc(atci_parse_cmd_param_t *parse_cmd);
static int htoi(unsigned char s[]);
#ifdef MTK_BUILD_SMT_LOAD
static bool rtc_alarm_test(int second);
static bool rtc_alarm_happened = false;
#endif
static void rtc_wait_busy(void);
static void rtc_write_trigger_wait(void);
static void rtc_set_rtc_mode(void);
static void rtc_clear_rtc_mode(void);
/*
AT+ERTCREG=<op>             |   "OK"
AT+ERTCREG=?                |   "+ERTCREG=(0,1)","OK"


*/
// AT command handler
atci_status_t atci_cmd_hdlr_rtc(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse = {{0}};
    uint16_t read_value = 0;
    int input_addr = 0;
    int input_value = 0;
    char* end_pos = NULL;
    hal_wdt_config_t wdt_config;

    LOG_I(RTC_ATCI, "atci_cmd_hdlr_rtc \r\n");


    resonse.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+ERTCREG=?
            strcpy((char *)resonse.response_buf, "+ERTCREG=(0,1)\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
        case ATCI_CMD_MODE_EXECUTION: // rec: AT+ERTCREG=<op>  the handler need to parse the parameters

            if (strncmp(parse_cmd->string_ptr, "AT+ERTCREG=0,", strlen("AT+ERTCREG=0,")) == 0) {
                /*command: AT+ERTCREG=0,1234*/
                end_pos = strchr(parse_cmd->string_ptr, ',');
                end_pos ++;

                input_addr = htoi((unsigned char *)end_pos);
                end_pos = NULL;
                LOG_I(RTC_ATCI, "read register address:0x%x\r\n", input_addr);

                /* read data of input register address */
                read_value = rtc_get_register(input_addr);

                snprintf((char *)resonse.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "+ERTCREG:0x%x,0x%x\r\n", input_addr, read_value);
                /* ATCI will help append "OK" at the end of resonse buffer  */
                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            } else if (strncmp(parse_cmd->string_ptr, "AT+ERTCREG=1,", strlen("AT+ERTCREG=1,")) == 0) {

                /*command: AT+ERTCREG=1,1234,456*/

                char* mid_pos = NULL;
                char str[20] = {0};
                mid_pos = strchr(parse_cmd->string_ptr, ',');
                mid_pos ++;
                end_pos = strchr(mid_pos, ',');
                memcpy(str, mid_pos, strlen(mid_pos) - strlen(end_pos));
                input_addr = htoi((unsigned char *)mid_pos);
                end_pos ++;
                input_value = htoi((unsigned char *)end_pos);

                mid_pos = NULL;
                end_pos = NULL;
                LOG_I(RTC_ATCI, "register address:0x%x, set register value:0x%x\r\n", input_addr, input_value);

                /* write input data to input register address*/
                rtc_set_register(input_addr, input_value);

                /* ATCI will help append "OK" at the end of resonse buffer  */
                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            }
#ifdef MTK_BUILD_SMT_LOAD
            else if (strncmp(parse_cmd->string_ptr, "AT+ERTC=", strlen("AT+ERTC=")) == 0) {

                /*command: AT+ERTC=3*/

                char* mid_pos = NULL;
                int second = 0;

                mid_pos = strchr(parse_cmd->string_ptr, '=');
                mid_pos++;
                second = atoi(mid_pos);
                LOG_I(RTC_ATCI, "second = %d\r\n", second);

                snprintf((char *)resonse.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "Please wait %d second(s) for RTC alarm.\r\n", second);
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
                if (rtc_alarm_test(second)) {
                    /* ATCI will help append "OK" at the end of resonse buffer  */
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    /* ATCI will help append "ERROR" at the end of resonse buffer   */
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }

                resonse.response_len = 0;
                atci_send_response(&resonse);
            } else if (strncmp(parse_cmd->string_ptr, "AT+ERTC32K", strlen("AT+ERTC32K")) == 0) {
                /*command: AT+ERTC32K*/
                int frequency = f32k_measure_clock(1, 1, 99);
                uint16_t value = rtc_read_osc32con();

                if (((32768-32768/100) < frequency) && (frequency < (32768+32768/100)) && ((value & 0x1000) != 0)) {
                    /* ATCI will help append "OK" at the end of resonse buffer  */
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    /* ATCI will help append "ERROR" at the end of resonse buffer   */
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }

                snprintf((char *)resonse.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "%d\r\n", frequency);
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            }
#endif
            else if (strstr((char *)parse_cmd->string_ptr, "AT+ERTCREG=2") != NULL) {
                /* set RTC_MODE */
                rtc_set_rtc_mode();
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);

                wdt_config.mode = HAL_WDT_MODE_RESET;
                wdt_config.seconds = 0;
                hal_wdt_init(&wdt_config);
                hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);
                //hal_wdt_software_reset();
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ERTCREG=3") != NULL) {
                /* clear RTC_MODE */
                rtc_clear_rtc_mode();
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);

                wdt_config.mode = HAL_WDT_MODE_RESET;
                wdt_config.seconds = 0;
                hal_wdt_init(&wdt_config);
                hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);
                //hal_wdt_software_reset();
            } else {
                /*invalide parameter, return "ERROR"*/
                strcpy((char *)resonse.response_buf, "ERROR\r\n");
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
            }

            break;

        default :
            /* others are invalid command format */
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }
    return ATCI_STATUS_OK;
}


// change the data with hex type in string to data with dec type
int htoi(unsigned char s[])
{
    int i;
    int n = 0;
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))
    {
        i = 2;
    }
    else
    {
        i = 0;
    }
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i)
    {
        if (tolower(s[i]) > '9')
        {
            n = 16 * n + (10 + tolower(s[i]) - 'a');
        }
        else
        {
            n = 16 * n + (tolower(s[i]) - '0');
        }
    }
    return n;
}

bool set_alarm_time(uint16_t year, uint8_t mon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec, uint8_t week) {
    hal_rtc_status_t ret;
    hal_rtc_time_t alarm;

    //The value of the RTC year is in a range from 0 to 127. The user has to define the base year and the RTC year is defined
    //as an offset. For example, define the base year to 2000 and assign 15 to RTC year to represent the year of 2015.
    alarm.rtc_year = year;
    alarm.rtc_mon = mon;
    alarm.rtc_day = day;
    alarm.rtc_hour = hour;
    alarm.rtc_min = min;
    alarm.rtc_sec = sec;
    alarm.rtc_week = week;

    //Set the RTC alarm time.
    ret = hal_rtc_set_alarm(&alarm);
    if(HAL_RTC_STATUS_OK != ret) {
        return false;
    }

    return true;
}

void alarm_handle_cb(void *user_data)
{
#ifdef MTK_BUILD_SMT_LOAD
    rtc_alarm_happened = true;
#endif
}

bool set_alarm_handle_cb() {
    hal_rtc_status_t ret;

    //Register a callback function to handle the RTC alarm.
    ret = hal_rtc_set_alarm_callback(alarm_handle_cb, NULL);
    if(HAL_RTC_STATUS_OK != ret) {
        return false;
    }

    //Enable an alarm notification.
    ret = hal_rtc_enable_alarm();
    if(HAL_RTC_STATUS_OK != ret) {
        return false;
    }

    return true;
}

bool get_current_time(uint8_t *year, uint8_t *mon, uint8_t *day, uint8_t *hour, uint8_t *min, uint8_t *sec, uint8_t *week) {
    hal_rtc_status_t ret;
    hal_rtc_time_t time;

    ret = hal_rtc_get_time(&time);
    if(HAL_RTC_STATUS_OK != ret) {
        return false;
    }

    //The value of the RTC year is in a range from 0 to 127. The user has to define the base year and the RTC year is defined
    //as an offset. For example, define the base year to 2000 and assign 15 to RTC year to represent the year of 2015.
    *year = time.rtc_year;
    *mon = time.rtc_mon;
    *day = time.rtc_day;
    *hour = time.rtc_hour;
    *min = time.rtc_min;
    *sec = time.rtc_sec;
    *week = time.rtc_week;

    return true;
}
#ifdef MTK_BUILD_SMT_LOAD
static void rtc_forward_time(hal_rtc_time_t *time, int second)
{
    int minute = 0;
    int hour = 0;
    int day = 0;
    int remender = 0;
    int max_day;
    const int days_in_month[13] = {
        0,  /* Null */
        31, /* Jan */
        28, /* Feb */
        31, /* Mar */
        30, /* Apr */
        31, /* May */
        30, /* Jun */
        31, /* Jul */
        31, /* Aug */
        30, /* Sep */
        31, /* Oct */
        30, /* Nov */
        31  /* Dec */
    };

    second += time->rtc_sec;
    minute = time->rtc_min;
    hour = time->rtc_hour;

    if (second > 59) {
        /* min */
        minute += second / 60;
        second %= 60;
    }
    time->rtc_sec = second;
    if (minute > 59) {
        /* hour */
        hour += minute / 60;
        minute %= 60;
    }
    time->rtc_min = minute;
    if (hour > 23) {
        /* day of week */
        day = hour / 24;
        hour %= 24;
        /* day of month */
        time->rtc_day += day;
        max_day = days_in_month[time->rtc_mon];
        if(time->rtc_mon == 2)
        {
            remender = time->rtc_year % 4;
            if(remender == 0)
                max_day++;
        }
        if(time->rtc_day > max_day)
        {
            time->rtc_day -= max_day;

            /* month of year */
            time->rtc_mon++;
            if(time->rtc_mon > 12) {
                time->rtc_mon = 1;
                time->rtc_year++;
            }
        }
    }
    time->rtc_hour = hour;
}

static bool rtc_alarm_test(int second)
{
    hal_rtc_time_t time;

    if(HAL_RTC_STATUS_OK != hal_rtc_init()) {
        LOG_E(RTC_ATCI, "hal_rtc_init fail");
        return false;
    }

    if (false == get_current_time(&time.rtc_year, &time.rtc_mon, &time.rtc_day, &time.rtc_hour, &time.rtc_min, &time.rtc_sec, &time.rtc_week)) {
        LOG_E(RTC_ATCI, "get_current_time fail");
        return false;
    }
    rtc_forward_time(&time, second);
    rtc_alarm_happened = false;
    if (false == set_alarm_time(time.rtc_year, time.rtc_mon, time.rtc_day, time.rtc_hour, time.rtc_min, time.rtc_sec, time.rtc_week)) {
        LOG_E(RTC_ATCI, "set_alarm_time fail");
        return false;
    }
    if (false == set_alarm_handle_cb()) {
        LOG_E(RTC_ATCI, "set_alarm_handle_cb fail");
        return false;
    }

    second += 10;
    do {
        //hal_gpt_delay_us(1000000);
        vTaskDelay(1000);
    } while ((rtc_alarm_happened == false) && second-- > 0);

    if (false == rtc_alarm_happened)
        LOG_E(RTC_ATCI, "rtc_alarm_test timeout");

    return rtc_alarm_happened;
}
#endif

static void rtc_wait_busy(void)
{
    uint32_t count = 0;
    while (count < 0x6EEEEE) {
        if (((*(volatile uint16_t *)0xA21e0000) & (0x40)) == 0) {
            break;
        }
        count++;
    }
}

static void rtc_write_trigger_wait(void)
{
    *(volatile uint16_t *)0xA21e0074 = 0x1;
    rtc_wait_busy();
}

static void rtc_set_rtc_mode(void)
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
        LOG_E(RTC_ATCI, "rtc_mode set fail\r\n");
    } else {
        LOG_E(RTC_ATCI, "rtc_mode set ok\r\n");
    }
}

static void rtc_clear_rtc_mode(void)
{
    uint16_t value = 0;
    value = *(volatile uint16_t *)0xA21e0078;
    value &= (~0x100);
    *(volatile uint16_t *)0xA21e0078 = value;
    rtc_write_trigger_wait();

    value = *(volatile uint16_t *)0xA21e0078;
    if ((value & 0x100) != 0) {
        LOG_E(RTC_ATCI, "rtc_mode clear fail\r\n");
    } else {
        LOG_E(RTC_ATCI, "rtc_mode clear ok\r\n");
    }
}

#endif
