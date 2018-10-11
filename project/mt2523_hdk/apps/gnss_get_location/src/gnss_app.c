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

#include <string.h>
#include <stdlib.h>
#include "stdio.h"
#include "gnss_api.h"
#include "gnss_driver.h"
#include <assert.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
//#include "utils.h"
#include "gnss_app.h"
#include "event_groups.h"
#include "gnss_ring_buffer.h"
#include "gnss_timer.h"
#include "gnss_log.h"
#include "hal_eint.h"
#include "hal_sleep_manager.h"

#include "epo_demo.h"
// This option is used to enable connect powerGPS tool via UART
// This feature is only for debug usage, and will impact the power consumption.
// You can disable it when you don't need it.
#ifdef GNSS_SUPPORT_TOOL_BRIDGE
#include "gnss_bridge.h"
#endif

#include "hal_rtc.h"
#include "task_def.h"

//************************* Customize Configration Begin **************************/

#define GNSS_SEND_RING_BUF_SIZE (4*1024)
#define GNSS_RECIEVE_RING_BUF_SIZE (16*1024)
#define GNSS_SEND_TEMP_BUF 256
#define GNSS_RECIEVE_TEMP_BUF 256

#define GNSS_NEMA_SENTENCE_BUF_LEN 256
#define GNSS_QUEUE_SIZE 40

#define GNSS_LOW_POWER_MODE_NORMAL 60
#define GNSS_LOW_POWER_MODE_ON_OFF (5 * 60)
#define GNSS_LOW_POWER_ON_OFF_MODE_GAP 60

#define GNSS_VALID_ACCURACY 150

// event group mask bit
#define GNSS_WAIT_SEND_BIT 0x01
#define GNSS_WAIT_SEND_BY_OTHER_TASK 0x02

#define GNSS_WAKE_LOCK_TIME_MS 50

#define GNSS_RTC_BASE_YEAR 2000
#define GNSS_FIX_INTERVAL 1

#define GNSS_MS_IN_ONE_SECOND 1000
//************************** Customize Configration End ****************************/


static struct {
    QueueHandle_t gnss_task_queue;
    EventGroupHandle_t gnss_event_group;
    ring_buf_struct_t recieve_ring_buf;
    ring_buf_struct_t send_ring_buf;
} gnss_task_cntx;

/*GNSS data structure */

typedef struct{
    int32_t periodic;
    gnss_location_handle handle;
} gnss_config_struct_t;

typedef enum {
    GNSS_LOCATION_STATE_WAITING_INIT,
    GNSS_LOCATION_STATE_INIT,
    GNSS_LOCATION_STATE_START,
    GNSS_LOCATION_STATE_STOP
} gnss_location_state_enum_t;

typedef enum {
    GNSS_LOCATION_MODE_NONE_PERIODIC, // GNSS will power off when fixed once.
    GNSS_LOCATION_MODE_NORMAL_PERIODIC, //GNSS fix interval is 1 HZ, and report location according to user defined fix interval.
    GNSS_LOCATION_MODE_LLE, //usd GNSS peroidic mode,  and report location according to user defined fix interval
    GNSS_LOCATION_MODE_ON_OFF //Power on/off GNSS chip.
} gnss_location_mode_t;

typedef struct {
    gnss_sentence_info_t nmea_sentence;
    gnss_location_struct_t location_data;
    gnss_config_struct_t config;
    gnss_location_state_enum_t state;
    int32_t on_off_mode_timer_id;
    int32_t periodic_timer_id;
    gnss_location_mode_t mode;
    int32_t sleep_timer;
    uint8_t sleep_handler;
    
// This option is used to enable connect powerGPS tool via UART
// This feature is only for debug usage, and will impact the power consumption.
// You can disable it when you don't need it.
#ifdef GNSS_SUPPORT_TOOL_BRIDGE
    TaskHandle_t bridge_task_handler;
#endif
} gnss_context_struct_t;


static gnss_context_struct_t g_gnss_location_context;

/**
* @brief       Parse location from NMEA sentence.
* @param[in]   input: NMEA sentence
* @param[in]   output: location
* @return      int32_t
*/
static int32_t gnss_get_location (gnss_sentence_info_t *input, gnss_location_struct_t *output)
{
    uint8_t *gpgga = input->GPGGA;
    uint8_t *gpacc = input->GPACC;
    int32_t i = 0;
    int32_t j = 0;
    int32_t flag = 0;
    int8_t accuracy[10] = {0};
    output->accuracy = 0xffff;
    output->latitude[0] = '\0';
    output->longitude[0] = '\0';
    memcpy(&output->nmea_sentence, input, sizeof(gnss_sentence_info_t));
    for (i = 0; i < GNSS_MAX_GPGGA_SENTENCE_LENGTH; i++) {
        if (gpgga[i] == ',') {
            if (gpgga[i + 1] == 'N') {
                int32_t len = i - j - 1;
                output->latitude[len] = 0;
                while (len > 0) {
                    output->latitude[len - 1] = gpgga[j + len];
                    len--;
                }
            }
            if (gpgga[i + 1] == 'E') {
                int32_t len = i - j - 1;
                if (gpgga[i + 3] == '0') {
                    GNSSLOGD("[GNSS Demo] Get location, invalid data!\n");
                    output->longitude[0] = 0;
                    output->latitude[0] = 0;
                    return 1;
                }
                output->longitude[len] = 0;
                while (len > 0) {
                    output->longitude[len - 1] = gpgga[j + len];
                    len--;
                }
                break;
            }
            if (gpgga[i + 1] == 'S') {
                int32_t len = i - j - 1;
                output->latitude[0] = '-';
                output->latitude[len + 1] = 0;
                while (len > 0) {
                    output->latitude[len] = gpgga[j + len];
                    len--;
                }
            }
            if (gpgga[i + 1] == 'W') {
                int32_t len = i - j - 1;
                if (gpgga[i + 3] == '0') {
                    GNSSLOGD("[GNSS Demo] Get location, invalid data!\n");
                    output->longitude[0] = 0;
                    output->latitude[0] = 0;
                    return 1;
                }
                output->longitude[0] = '-';
                output->longitude[len + 1] = 0;
                while (len >= 0) {
                    output->longitude[len] = gpgga[j + len];
                    len--;
                }
                break;
            }
            j = i;
        }
    }
    if (output->latitude[0] == 0 && output->longitude[0] == 0) {
        return 1;
    }
    j = 0;
    for (i = 0; i < GNSS_MAX_GPACC_SENTENCE_LENGHT; i++) {
        if (gpacc[i] == '*') {
            accuracy[j] = '\0';
            break;
        } else if (flag) {
            accuracy[j++] = gpacc[i];
        } else if (gpacc[i] == ',') {
            flag = 1;
        }
    }
    output->accuracy = atoi((char*) accuracy);
    GNSSLOGD("[GNSS Demo] Get location, latitude:%s, longitude:%s, accuracy:%d\n", output->latitude, output->longitude, (int) output->accuracy);
    return 0;
}

/**
* @brief This function is used to process PMTK command, especially for PMTK response about 663.
* @param[in] nmea_buf: is point to a buffer which contains NMEA sentence.
* @param[in] buf_len: NMEA sentence length.
* @param[in] nmea_param: Parsed parameter list
* @param[in] buf_len: Parmeter number.
* @return void
*/
static void gnss_get_sentence_param(int8_t* nmea_buf, int32_t buf_len, int8_t* nmea_param[], int32_t* param_num)
{
    int32_t ind = 0;
    int32_t ret_num = 0;
    while(ind < buf_len) {
        if (nmea_buf[ind] == ',') {
            nmea_param[ret_num++] = nmea_buf + ind + 1;
            nmea_buf[ind] = 0;
        } else if (nmea_buf[ind] == '*') {
            nmea_buf[ind] = 0;
            break;
        }
        ind ++;
    }
    *param_num = ret_num;
}

/**
* @brief This function is used to process PMTK command, especially for PMTK response about 663.
* @param[in] nmea_buf: is point to a buffer which contains NMEA sentence.
* @param[in] buf_len: NMEA sentence length.
* @return void
*/
static void gnss_process_pmtk_response(int8_t*nmea_buf, int32_t buf_len)
{
    int8_t* nmea_param[60];
    int32_t param_num;

    gnss_get_sentence_param(nmea_buf, buf_len, nmea_param, &param_num);

    GNSSLOGD("gnss_process_pmtk_response, %s\n", nmea_buf);
    if ( strstr((char*) nmea_buf, "PMTK010") ) {
        if ( strstr((char*) nmea_param[0], "002") ) {
//Enable this option to download EPO via bt notification service.
//If you don't need it, you can disable this option in gnss_app.h
#ifdef GNSS_SUPPORT_EPO_DOWNLOAD_BY_BT
            hal_rtc_time_t rtc_time = {0};
            GNSSLOGD("Aiding EPO\n");
            hal_rtc_get_time(&rtc_time);
            epo_demo_send_assistance_data(rtc_time.rtc_year + GNSS_RTC_BASE_YEAR,rtc_time.rtc_mon,rtc_time.rtc_day,rtc_time.rtc_hour);
#endif
        }
    }
}

/**
* @brief This function used to save nema data which comes from GNSS chip.
* @param[in] nmea_buf is pointer to a buffer which contain NMEA sentence.
* @param[in] NMEA sentence length.
* @return void
*/
static void gnss_nmea_data_process(int8_t* nmea_buf, int32_t buf_len) {
    if (strstr((char*) nmea_buf, "GPGGA")) {
        memset(&g_gnss_location_context.nmea_sentence, 0, sizeof(g_gnss_location_context.nmea_sentence));
        memcpy(g_gnss_location_context.nmea_sentence.GPGGA, nmea_buf, buf_len < GNSS_MAX_GPGGA_SENTENCE_LENGTH ? buf_len : GNSS_MAX_GPGGA_SENTENCE_LENGTH);
    } else if (strstr((char*) nmea_buf, "GPGSA")) {
        memcpy(g_gnss_location_context.nmea_sentence.GPGSA, nmea_buf, buf_len < GNSS_MAX_GPGSA_SENTENCE_LENGTH ? buf_len : GNSS_MAX_GPGSA_SENTENCE_LENGTH);
    } else if (strstr((char*) nmea_buf, "GPRMC")) {
        memcpy(g_gnss_location_context.nmea_sentence.GPRMC, nmea_buf, buf_len < GNSS_MAX_GPRMC_SENTENCE_LENGTH ? buf_len : GNSS_MAX_GPRMC_SENTENCE_LENGTH);
    } else if (strstr((char*) nmea_buf, "GPVTG")) {
        memcpy(g_gnss_location_context.nmea_sentence.GPVTG, nmea_buf, buf_len < GNSS_MAX_GPVTG_SENTENCE_LENGTH ? buf_len : GNSS_MAX_GPVTG_SENTENCE_LENGTH);
    } else if (strstr((char*) nmea_buf, "GPGSV")) {
        memcpy(g_gnss_location_context.nmea_sentence.GPGSV + strlen((char*)g_gnss_location_context.nmea_sentence.GPGSV), nmea_buf, (buf_len + strlen((char*)g_gnss_location_context.nmea_sentence.GPGSV)) < GNSS_MAX_GPGSV_SENTENCE_LENGTH ? buf_len : (GNSS_MAX_GPGSV_SENTENCE_LENGTH - strlen((char*)g_gnss_location_context.nmea_sentence.GPGSV)));
    } else if (strstr((char*) nmea_buf, "GLGSV")) {
        memcpy(g_gnss_location_context.nmea_sentence.GLGSV, nmea_buf, buf_len < GNSS_MAX_GLGSV_SENTENCE_LENGTH ? buf_len : GNSS_MAX_GLGSV_SENTENCE_LENGTH);
    } else if (strstr((char*) nmea_buf, "GLGSA")) {
        memcpy(g_gnss_location_context.nmea_sentence.GLGSA, nmea_buf, buf_len < GNSS_MAX_GLGSA_SENTENCE_LENGTH ? buf_len : GNSS_MAX_GLGSA_SENTENCE_LENGTH);
    } else if (strstr((char*) nmea_buf, "BDGSV")) {
        memcpy(g_gnss_location_context.nmea_sentence.BDGSV, nmea_buf, buf_len < GNSS_MAX_BDGSV_SENTENCE_LENGTH ? buf_len : GNSS_MAX_BDGSV_SENTENCE_LENGTH);
    } else if (strstr((char*) nmea_buf, "BDGSA")) {
        memcpy(g_gnss_location_context.nmea_sentence.BDGSA, nmea_buf, buf_len < GNSS_MAX_BDGSA_SENTENCE_LENGTH ? buf_len : GNSS_MAX_BDGSA_SENTENCE_LENGTH);
    } else if (strstr((char*) nmea_buf, "GPACCURACY")) {
        memcpy(g_gnss_location_context.nmea_sentence.GPACC, nmea_buf, buf_len < GNSS_MAX_GPACC_SENTENCE_LENGHT ? buf_len : GNSS_MAX_GPACC_SENTENCE_LENGHT);
    } else if (strstr((char*) nmea_buf, "PMTK")) {
        gnss_process_pmtk_response(nmea_buf, buf_len);
    }
}

/**
* @brief Read NMEA sentence from GNSS module.
* @return void
*/
static void gnss_recieve_data()
{
    static int8_t sentence_buf[GNSS_RECIEVE_TEMP_BUF];
    int32_t sentence_length;
    while (1) {
        sentence_length = gnss_read_sentence(sentence_buf, GNSS_RECIEVE_TEMP_BUF);
        if (sentence_length > 0) {
            sentence_buf[sentence_length] = 0;
            GNSSLOGD("gnss_recieve_data:%s\n", sentence_buf);
            gnss_nmea_data_process(sentence_buf, sentence_length);
        } else {
            return;
        }
    }
}

/**
* @brief Timer callback, used to control GNSS chip power on/off, and report location to user.
* @param[in] timer_id: timer id
* @return void
*/
static void gnss_on_off_timer_handle_func(int32_t timer_id)
{
    static int32_t on_off_flag;
    
    GNSSLOGD("gnss on off handle, is_on:%d,timer_id:%d\n", (int)on_off_flag, timer_id);
    if (timer_id == g_gnss_location_context.on_off_mode_timer_id) {
        if (on_off_flag == 0) {
            on_off_flag = 1;
            gnss_power_on();
            g_gnss_location_context.on_off_mode_timer_id = gnss_start_timer(GNSS_LOW_POWER_ON_OFF_MODE_GAP * 1000, gnss_on_off_timer_handle_func);
        } else {
            on_off_flag = 0;
            gnss_power_off();
            g_gnss_location_context.on_off_mode_timer_id =
            gnss_start_timer((g_gnss_location_context.config.periodic - GNSS_LOW_POWER_ON_OFF_MODE_GAP) * 1000,
            gnss_on_off_timer_handle_func);

            memset(&g_gnss_location_context.location_data, 0, sizeof(gnss_location_struct_t));
            gnss_get_location(&g_gnss_location_context.nmea_sentence, &g_gnss_location_context.location_data);
            if (g_gnss_location_context.config.handle != NULL) {
                g_gnss_location_context.config.handle(GNSS_LOCATION_HANDLE_TYPE_DATA, &g_gnss_location_context.location_data);
            }
        }
    }
}

/**
* @brief Timer callback, used to report location to user.
* @param[in] timer_id: timer id
* @return void
*/
static void gnss_periodic_timer_handle_func(int32_t timer_id) {
    GNSSLOGD("gnss_periodic_timer_handle_func, timer_id:%d\n", (int)timer_id);
    if (timer_id == g_gnss_location_context.periodic_timer_id) {

        memset(&g_gnss_location_context.location_data, 0, sizeof(gnss_location_struct_t));
        gnss_get_location(&g_gnss_location_context.nmea_sentence, &g_gnss_location_context.location_data);
        if (g_gnss_location_context.config.handle != NULL) {
            g_gnss_location_context.config.handle(GNSS_LOCATION_HANDLE_TYPE_DATA, &g_gnss_location_context.location_data);
        }
    }
}

/**
* @brief Send PMTK command to gnss chip, help to construct '$' + buf + '*" + checksum + '\r' + '\n'.
* @param[in] timer_id: timer id
* @return void
*/
void gnss_app_send_cmd(int8_t* buf, int32_t buf_len)
{
    const int32_t wait_ticket = 0xF0;
    int32_t ret_len = 0;
    int8_t temp_buf[GNSS_NEMA_SENTENCE_BUF_LEN];
    int8_t* ind;
    uint8_t checkSumL = 0, checkSumR;

    GNSSLOGD("gnss_app_send_cmd:%s\n", buf);

    if (buf_len + 6 > GNSS_NEMA_SENTENCE_BUF_LEN) {
        return;
    }

    ind = buf;
    while(ind - buf < buf_len) {
        checkSumL ^= *ind;
        ind++;
    }
    temp_buf[0] = '$';
    memcpy(temp_buf + 1, buf, buf_len);
    temp_buf[buf_len + 1] = '*';

    checkSumR = checkSumL & 0x0F;
    checkSumL = (checkSumL >> 4) & 0x0F;
    temp_buf[buf_len + 2] = checkSumL >= 10 ? checkSumL + 'A' - 10 : checkSumL + '0';
    temp_buf[buf_len + 3] = checkSumR >= 10 ? checkSumR + 'A' - 10 : checkSumR + '0';
    temp_buf[buf_len + 4] = '\r';
    temp_buf[buf_len + 5] = '\n';
    temp_buf[buf_len + 6] = '\0';
    buf_len += 6;
    while (1) {
        ret_len += gnss_send_command(temp_buf + ret_len, buf_len - ret_len);
        if (ret_len == buf_len) {
            return;
        }
        xEventGroupWaitBits(gnss_task_cntx.gnss_event_group,
            GNSS_WAIT_SEND_BIT,
            pdTRUE,
            pdTRUE,
            wait_ticket);
    }
}

/**
* @brief parse PMTK740 to get time.
* @param[in] buf: Command buffer
* @param[in] buf_len: Command length
* @return void
*/
void set_time_by_pmtk_process(int8_t* buf, uint32_t buf_len)
{
    static int8_t pmtkbuf[GNSS_NEMA_SENTENCE_BUF_LEN];
    int8_t* nmea_param[60];
    int32_t param_num;
    hal_rtc_time_t rtc_time = {0};
    GNSSLOGD("set_time_by_pmtk_process\n");

    if (buf_len >= GNSS_NEMA_SENTENCE_BUF_LEN) {
        return;
    }
    memcpy(pmtkbuf, buf, buf_len);
    gnss_get_sentence_param(pmtkbuf, buf_len, nmea_param, &param_num);
    if (param_num == 6) {
        rtc_time.rtc_year = atoi((char*) nmea_param[0]) % 100;
        rtc_time.rtc_mon = atoi((char*) nmea_param[1]);
        rtc_time.rtc_day = atoi((char*) nmea_param[2]);
        rtc_time.rtc_hour = atoi((char*) nmea_param[3]);
        rtc_time.rtc_min = atoi((char*) nmea_param[4]);
        rtc_time.rtc_sec = atoi((char*) nmea_param[5]);
        GNSSLOGD("get rtc:%d-%d-%d %d:%d:%d\n", 
            rtc_time.rtc_year,
            rtc_time.rtc_mon,
            rtc_time.rtc_day,
            rtc_time.rtc_hour,
            rtc_time.rtc_min,
            rtc_time.rtc_sec);
        hal_rtc_set_time(&rtc_time);
    } else {
        GNSSLOGD("wrong PMTK:%s\n", buf);
    }
    
}

/**
* @brief Send PMTK command to gnss chip, can be trigger by other task.
* @param[in] buf: Command buffer
* @param[in] buf_len: Command length
* @return void
*/
void gnss_app_forward_cmd(int8_t* buf, uint32_t buf_len)
{
    gnss_message_struct_t message;
    message.message_id = GNSS_ENUM_SEND_COMMAND;
    message.param1 = buf_len;
    message.param2 = buf;
    GNSSLOGD("gnss_app_forward_cmd\r\n");
    if (xQueueSend(gnss_task_cntx.gnss_task_queue, &message, 0) == pdPASS) {
        GNSSLOGD("send suceed\r\n");
        xEventGroupWaitBits(gnss_task_cntx.gnss_event_group,
            GNSS_WAIT_SEND_BY_OTHER_TASK,
            pdTRUE,
            pdTRUE,
            100);
    }
}

/**
* @brief Process command send by other task.
* @param[in] buf: Command buffer
* @param[in] buf_len: Command length
* @return void
*/
static void gnss_app_forward_cmd_helper(int8_t* buf, int32_t buf_len)
{
    int ret_len = 0;
    GNSSLOGD("gnss_app_forward_cmd_helper,buf:%s\r\n", (char*) buf);
    if (strncmp((char*)buf, "$PMTK740", strlen("$PMTK740")) == 0) {
        set_time_by_pmtk_process(buf, buf_len);
    }
    while (1) {
        ret_len += gnss_send_command(buf + ret_len, buf_len - ret_len);
        if (ret_len == buf_len) {
            break;
        }
    }
    xEventGroupSetBits(gnss_task_cntx.gnss_event_group, GNSS_WAIT_SEND_BY_OTHER_TASK);
}


/**
* @brief Set GNSS periodic mode
* @return void
*/
static void gnss_set_preport_and_fix_period()
{
    int8_t strbuf[200];
    int run_time, sleep_time;
    if (g_gnss_location_context.mode == GNSS_LOCATION_MODE_LLE) {
        run_time = g_gnss_location_context.config.periodic >> 2;
        sleep_time = g_gnss_location_context.config.periodic - run_time;
        sprintf((char*) strbuf, "PMTK225,2,%d,%d,%d,%d", run_time*GNSS_MS_IN_ONE_SECOND, sleep_time*GNSS_MS_IN_ONE_SECOND, run_time*GNSS_MS_IN_ONE_SECOND, sleep_time*GNSS_MS_IN_ONE_SECOND);
        gnss_app_send_cmd(strbuf, strlen((char*) strbuf));
    } else {
        gnss_app_send_cmd((int8_t*) "PMTK225,0,0,0", strlen("PMTK225,0,0,0"));//Disable GNSS periodic mode
    }
}

/**
* @brief Callback function, this funcion is callback by others, not running in GNSS app task, only should be pass the message back to GNSS app task.
* @param[in]    type: GNSS module event
* @param[in]    param: Callback parameter
* @return void
*/
static void gnss_driver_callback_func(gnss_notification_type_t type, void *param)
{
    gnss_message_struct_t gnss_message;
    BaseType_t xHigherPriorityTaskWoken;
// This option is used to enable connect powerGPS tool via UART
// This feature is only for debug usage, and will impact the power consumption.
// You can disable it when you don't need it.
#ifdef GNSS_SUPPORT_TOOL_BRIDGE
    gnss_debug_data_t* debug_info;
#endif
    switch (type) {
        case GNSS_NOTIFICATION_TYPE_POWER_ON_CNF:
        // If any error when power on, you can send error msg here.
            gnss_message.message_id = GNSS_ENUM_POWER_ON_CNF;
            break;
        case GNSS_NOTIFICATION_TYPE_POWER_OFF_CNF:
            gnss_message.message_id = GNSS_ENUM_POWER_OFF_CNF;
            break;
        case GNSS_NOTIFICATION_TYPE_READ:
            gnss_message.message_id = GNSS_ENUM_READY_TO_READ;
            break;
        case GNSS_NOTIFICATION_TYPE_WRITE:
            xEventGroupSetBitsFromISR(gnss_task_cntx.gnss_event_group, GNSS_WAIT_SEND_BIT, &xHigherPriorityTaskWoken);
            return;
        case GNSS_NOTIFICATION_TYPE_DEBUG_INFO:
            // Process debug data
            // This option is used to enable connect powerGPS tool via UART
            // This feature is only for debug usage, and will impact the power consumption.
            // You can disable it when you don't need it.
            #ifdef GNSS_SUPPORT_TOOL_BRIDGE
            debug_info = (gnss_debug_data_t *) param;
            //GNSSLOGD("%s", (char*) debug_info->info_data);
            gnss_bridge_put_data((uint8_t*)debug_info->debug_data, (uint32_t)debug_info->length);
			#endif
            return;
        case GNSS_NOTIFICATION_TYPE_HOST_WAKEUP:
            gnss_message.message_id = GNSS_ENUM_HOST_WAKEUP;
            break;
        default:
            break;
    }
    xQueueSendFromISR(gnss_task_cntx.gnss_task_queue, &gnss_message, &xHigherPriorityTaskWoken);
}

/**
* @brief Timeout callback, send timeout event to GNSS app task.
* @return void
*/
static void gnss_timer_expiry_notify(void)
{
    gnss_message_struct_t gnss_message;
    gnss_message.message_id = GNSS_ENUM_TIME_EXPIRY;
    xQueueSendFromISR(gnss_task_cntx.gnss_task_queue, ( void * ) &gnss_message, 0);
}

/**
* @brief Timeout callback, send timeout event to GNSS app task.
* @return void
*/
static void gnss_sleep_timer_handle_func(int32_t timer_id)
{
    GNSSLOGD("gnss_sleep_timer_handle_func\r\n");
    g_gnss_location_context.sleep_timer = -1;
    hal_sleep_manager_unlock_sleep(g_gnss_location_context.sleep_handler);
    hal_eint_unmask((hal_eint_number_t) BSP_GNSS_EINT);
}

/**
* @brief Restart sleep timer
* @return void
*/
static void gnss_sleep_restart_timer()
{
    GNSSLOGD("gnss_sleep_restart_timer\r\n");
    if (g_gnss_location_context.sleep_timer != -1) {
        gnss_stop_timer(g_gnss_location_context.sleep_timer);
        g_gnss_location_context.sleep_timer = gnss_start_timer(GNSS_WAKE_LOCK_TIME_MS, gnss_sleep_timer_handle_func);
    }
}

/**
* @brief Host wakeup handle, lock sleep.
* @return void
*/
static void gnss_wakeup_handle()
{
    GNSSLOGD("gnss_wakeup_handle\r\n");
    hal_sleep_manager_lock_sleep(g_gnss_location_context.sleep_handler);
    gnss_app_send_cmd((int8_t*) "PMTK850", strlen("PMTK850"));
    if (g_gnss_location_context.sleep_timer == -1) {
        g_gnss_location_context.sleep_timer = gnss_start_timer(GNSS_WAKE_LOCK_TIME_MS, gnss_sleep_timer_handle_func);
    }
}


/**
* @brief Process GNSS message
* @param[in]    message: GNSS message
* @return void
*/
static void gnss_task_msg_handler(gnss_message_struct_t *message)
{
    static int32_t is_power_on;
    
    if (!message) {
        return;
    }
    switch (message->message_id) {
        case GNSS_ENUM_POWER_ON_REQ:
            GNSSLOGD("GNSS_ENUM_POWER_ON_REQ:%d\n", g_gnss_location_context.config.periodic);
            if (g_gnss_location_context.config.periodic >= GNSS_LOW_POWER_MODE_ON_OFF) {
                g_gnss_location_context.mode = GNSS_LOCATION_MODE_ON_OFF;
                g_gnss_location_context.on_off_mode_timer_id =
                    gnss_start_timer((g_gnss_location_context.config.periodic - GNSS_LOW_POWER_ON_OFF_MODE_GAP) * 1000,
                    gnss_on_off_timer_handle_func);
            } else {
                if (g_gnss_location_context.config.periodic == 0) {
                    g_gnss_location_context.mode = GNSS_LOCATION_MODE_NONE_PERIODIC;
                } else if (g_gnss_location_context.config.periodic < GNSS_LOW_POWER_MODE_NORMAL) {
                    g_gnss_location_context.mode = GNSS_LOCATION_MODE_NORMAL_PERIODIC;
                } else if (g_gnss_location_context.config.periodic < GNSS_LOW_POWER_MODE_ON_OFF) {
                    g_gnss_location_context.mode = GNSS_LOCATION_MODE_LLE;
                }
                gnss_power_on();
            }
            if (g_gnss_location_context.mode != GNSS_LOCATION_MODE_NONE_PERIODIC) {
                g_gnss_location_context.periodic_timer_id = gnss_start_repeat_timer(g_gnss_location_context.config.periodic * 1000, gnss_periodic_timer_handle_func);
            }
            is_power_on = 1;
            break;
        case GNSS_ENUM_POWER_OFF_REQ:
            GNSSLOGD("GNSS_ENUM_POWER_OFF_REQ\n");
            gnss_power_off();
            if (g_gnss_location_context.periodic_timer_id != -1) {
                gnss_stop_timer(g_gnss_location_context.periodic_timer_id);
                g_gnss_location_context.periodic_timer_id = -1;
            }
            if (g_gnss_location_context.on_off_mode_timer_id != -1) {
                gnss_stop_timer(g_gnss_location_context.on_off_mode_timer_id);
                g_gnss_location_context.on_off_mode_timer_id = -1;
            }
            is_power_on = 0;
            g_gnss_location_context.state = GNSS_LOCATION_STATE_STOP;
            break;
        case GNSS_ENUM_CONFIG_REQ:
            GNSSLOGD("GNSS_ENUM_CONFIG_REQ:%d,%d\n", is_power_on, (int32_t)message->param1);
            if (is_power_on == 0) {
                g_gnss_location_context.config.handle = (gnss_location_handle) message->param2;
                g_gnss_location_context.config.periodic = (int32_t)message->param1;
            } else {
                gnss_power_off();
                g_gnss_location_context.config.handle = (gnss_location_handle) message->param2;
                g_gnss_location_context.config.periodic = message->param1;
                if (g_gnss_location_context.on_off_mode_timer_id != -1) {
                    gnss_stop_timer(g_gnss_location_context.on_off_mode_timer_id);
                    g_gnss_location_context.on_off_mode_timer_id = -1;
                }
                if (g_gnss_location_context.periodic_timer_id != -1) {
                    gnss_stop_timer(g_gnss_location_context.periodic_timer_id);
                    g_gnss_location_context.periodic_timer_id = -1;
                }
                gnss_demo_app_start();
            }
            break;
        case GNSS_ENUM_POWER_ON_CNF:
            GNSSLOGD("GNSS_ENUM_POWER_ON_CNF\n");
            g_gnss_location_context.state = GNSS_LOCATION_STATE_START;
            if (is_power_on) {
                 gnss_set_preport_and_fix_period(); 
            }
            break;
        case GNSS_ENUM_READY_TO_READ:
            gnss_sleep_restart_timer();
            gnss_recieve_data();
            break;
        case GNSS_ENUM_READY_TO_WRITE:
            // currently no use, because the data send is blocking api.
            break;
        case GNSS_ENUM_TIME_EXPIRY:
            excute_timer();
            break;
        case GNSS_ENUM_SEND_COMMAND:
            GNSSLOGD("GNSS_ENUM_SEND_COMMAND\n");
            gnss_app_forward_cmd_helper((int8_t*)message->param2, (int32_t)message->param1);
            break;
        case GNSS_ENUM_HOST_WAKEUP:
            gnss_wakeup_handle();
            break;
        default:
            break;
    }
}


/**
* @brief GNSS app init
* @return void
*/
static void gnss_task_init() {
    static int8_t send_ring_buf[GNSS_SEND_RING_BUF_SIZE];
    static int8_t recieve_ring_buf[GNSS_RECIEVE_RING_BUF_SIZE];
    g_gnss_location_context.on_off_mode_timer_id = -1;
    g_gnss_location_context.periodic_timer_id = -1;
    g_gnss_location_context.config.periodic = 1;
    g_gnss_location_context.config.handle = NULL;
    g_gnss_location_context.state = GNSS_LOCATION_STATE_WAITING_INIT;
    gnss_init(gnss_driver_callback_func);
    gnss_timer_init(gnss_timer_expiry_notify);
    ring_buf_init(&gnss_task_cntx.send_ring_buf, send_ring_buf, GNSS_SEND_RING_BUF_SIZE);
    ring_buf_init(&gnss_task_cntx.recieve_ring_buf, recieve_ring_buf, GNSS_RECIEVE_RING_BUF_SIZE);
    g_gnss_location_context.sleep_handler = hal_sleep_manager_set_sleep_handle("GSH");
    g_gnss_location_context.sleep_timer = -1;
    GNSSLOGD("gnss get sleep handle: %d\r\n", g_gnss_location_context.sleep_handler);
}


/**
* @brief GNSS main loop.
* @return void
*/
static void gnss_task_main()
{
    gnss_message_struct_t queue_item;
    GNSSLOGD("gnss_task_main\n");

    g_gnss_location_context.state = GNSS_LOCATION_STATE_INIT;
    gnss_demo_app_config(GNSS_FIX_INTERVAL, NULL);
    gnss_demo_app_start();
    while (1) {
        if (xQueueReceive(gnss_task_cntx.gnss_task_queue, &queue_item, portMAX_DELAY)) {
            gnss_task_msg_handler(&queue_item);
        }
    }
}

/**
* @brief GNSS app entry function.
* @return void
*/
TaskHandle_t gnss_demo_app_create()
{
    TaskHandle_t task_handler;
    BaseType_t ret;
    GNSSLOGD("gnss_demo_app_create\n");
    gnss_task_init();
    gnss_task_cntx.gnss_task_queue = xQueueCreate( GNSS_QUEUE_SIZE, sizeof( gnss_message_struct_t ) );
    gnss_task_cntx.gnss_event_group = xEventGroupCreate();
// This option is used to enable connect powerGPS tool via UART
// This feature is only for debug usage, and will impact the power consumption.
// You can disable it when you don't need it.
#ifdef GNSS_SUPPORT_TOOL_BRIDGE
    //g_gnss_location_context.bridge_task_handler = gnss_bridge_task_init();
    gnss_bridge_atcmd_init();
    //gnss_bridge_port_reinit(SERIAL_PORT_DEV_USB_COM1);
#endif
    ret = xTaskCreate((TaskFunction_t) gnss_task_main, 
        GNSS_DEMO_TASK_NAME, 
        GNSS_DEMO_TASK_STACK_SIZE/(( uint32_t )sizeof( StackType_t )), 
        NULL, 
        GNSS_DEMO_TASK_PRIO, 
        &task_handler );
    GNSSLOGD("task handler:%d, create result:%d\n", task_handler, ret);
    if (ret != pdPASS) {
        assert(0);
    }
    return task_handler;
}

/**
* @brief Configrate GNSS woking period.
* @param[in]    fix_interval: fix interval in seconds, if equal to 0, fix once.
* @param[in]    handle: Call back function
* @return void
*/
void gnss_demo_app_config(int32_t fix_interval, gnss_location_handle handle)
{
    GNSSLOGD("gnss_demo_app_config\n");
    gnss_message_struct_t gnss_message;
    gnss_message.message_id = GNSS_ENUM_CONFIG_REQ;
    gnss_message.param1 = fix_interval;
    gnss_message.param2 = (void*) handle;
    xQueueSend(gnss_task_cntx.gnss_task_queue, &gnss_message, 0);
}

/**
* @brief Power on GNSS chip.
* @return void
*/
void gnss_demo_app_start()
{
    // Send power on msg to GNSS task.
    GNSSLOGD("gnss_demo_app_start\n");
    gnss_message_struct_t gnss_message;
    gnss_message.message_id = GNSS_ENUM_POWER_ON_REQ;
    xQueueSend(gnss_task_cntx.gnss_task_queue, &gnss_message, 0);
}

/**
* @brief Power off GNSS chip.
* @return void
*/
void gnss_demo_app_stop()
{
    GNSSLOGD("gnss_demo_app_stop\n");
    gnss_message_struct_t gnss_message;
    gnss_message.message_id = GNSS_ENUM_POWER_OFF_REQ;
    xQueueSend(gnss_task_cntx.gnss_task_queue, &gnss_message, 0);

    while (1){
        GNSSLOGD("waiting stats: %d\n", g_gnss_location_context.state);
        if (g_gnss_location_context.state == GNSS_LOCATION_STATE_STOP)
            break;
        vTaskDelay(10);
    }
}

/**
* @brief GNSS app destory function.
* @return void
*/
void gnss_demo_app_destroy(TaskHandle_t task_handler)
{
    GNSSLOGD("gnss_demo_app_destroy\n");
// This option is used to enable connect powerGPS tool via UART
// This feature is only for debug usage, and will impact the power consumption.
// You can disable it when you don't need it.
#ifdef GNSS_SUPPORT_TOOL_BRIDGE
    //gnss_bridge_task_deinit(g_gnss_location_context.bridge_task_handler);
#endif
    vEventGroupDelete(gnss_task_cntx.gnss_event_group);
    gnss_timer_deinit();
    vQueueDelete(gnss_task_cntx.gnss_task_queue);
    gnss_task_cntx.gnss_task_queue = 0;
    vTaskDelete(task_handler);
}


