/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation (MediaTek Software) are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. (MediaTek) and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek (License Agreement) and been granted explicit permission to do so within
 * the License Agreement (Permitted User).  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN AS-IS BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
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
#include <assert.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
//#include "utils.h"
#include "gnss_app.h"
#include "event_groups.h"
#include "ring_buf.h"
#include "gnss_timer.h"
#include "gnss_log.h"
#include "task_def.h"
#ifdef GNSS_SUPPORT_TOOL_BRIDGE
#include "gnss_bridge.h"
#endif

//************************* Customize Configration Begin **************************/

#define GNSS_SEND_RING_BUF_SIZE (4*1024)
#define GNSS_RECIEVE_RING_BUF_SIZE (16*1024)
#define GNSS_SEND_TEMP_BUF 256
#define GNSS_RECIEVE_TEMP_BUF 256
#define GNSS_DATA_RECIVE_BIT (1 << 0)
#define GNSS_DATA_SEND_BIT (1 << 1)
#define GNSS_NEMA_SENTENCE_BUF_LEN 256
#define GNSS_QUEUE_SIZE 10

#define GNSS_LOW_POWER_MODE_NORMAL 60
#define GNSS_LOW_POWER_MODE_ON_OFF (5 * 60)
#define GNSS_LOW_POWER_ON_OFF_MODE_GAP 60

#define GNSS_VALID_ACCURACY 150

#define GNSS_WAIT_SEND_BIT 0x01
#define GNSS_WAIT_SEND_BY_OTHER_TASK 0x02

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
    GNSS_LOCATION_MODE_NONE_PERIODIC,
    GNSS_LOCATION_MODE_NORMAL_PERIODIC,
    GNSS_LOCATION_MODE_LLE,
    GNSS_LOCATION_MODE_ON_OFF
} gnss_location_mode_t;

typedef struct {
    gnss_sentence_info_t nmea_sentence;
    gnss_location_struct_t location_data;
    gnss_config_struct_t config;
    gnss_location_state_enum_t state;
    int32_t on_off_mode_timer_id;
    int32_t periodic_timer_id;
    gnss_location_mode_t mode;
} gnss_context_struct_t;


static gnss_context_struct_t g_gnss_location_context;
extern void epo_demo_send_assistance_data(int iYr, int iMo, int iDay, int iHr);
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

    if ( strstr((char*) nmea_buf, "PMTK001") ) {
        if ( strstr((char*) nmea_param[0], "663") ) {
            if (atoi((char*) nmea_param[1]) == 3) {
                //
                if (atoi((char*) nmea_param[2]) == 0) {
                    //time ading
                }
                if (atoi((char*) nmea_param[3]) == 0) {
                    // loc ading
                }
                if (atoi((char*) nmea_param[4]) < 4 || atoi((char*) nmea_param[5]) < 4) {
                    // epo ading

                }
            }
        }
    }
}

/**
 * @brief this function used to save nema data which comes from GNSS chip
 * @param[in] nmea_buf is point to a buffer which contain nmea sentence.
 * @param[in] nmea sentence length.
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

static void gnss_recieve_data()
{
    static int8_t sentence_buf[GNSS_RECIEVE_TEMP_BUF];
    int32_t sentence_length;

    while (1) {
        sentence_length = gnss_read_sentence(sentence_buf, GNSS_RECIEVE_TEMP_BUF);
        if (sentence_length > 0) {
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
    //const int32_t wait_ticket = 0xFFFFFFFF;
    int32_t ret_len = 0;
    int8_t temp_buf[256];
    int8_t* ind;
    uint8_t checkSumL = 0, checkSumR;

    GNSSLOGD("gnss_app_send_cmd:%s\n", buf);

    if (buf_len + 6 > 256) {
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
        // uart api not ready, so waiting 3 ticket for data transfer
        vTaskDelay(3);
        if (ret_len == buf_len) {
            return;
        }

    }
}


/**
* @brief Send PMTK command to gnss chip, can be trigger by other task.
* @param[in] buf: Command buffer
* @param[in] buf_len: Command length
* @return void
*/
void gnss_app_send_cmd_by_other_task(int8_t* buf, uint32_t buf_len)
{
    gnss_message_struct_t message;
    message.message_id = GNSS_ENUM_SEND_COMMAND;
    message.param1 = buf_len;
    message.param2 = buf;
    xQueueSend(gnss_task_cntx.gnss_task_queue, &message, 0);
    xEventGroupWaitBits(gnss_task_cntx.gnss_event_group, 
        GNSS_WAIT_SEND_BY_OTHER_TASK, 
        pdTRUE, 
        pdTRUE,
        100);
}

/**
* @brief Process command send by other task.
* @param[in] buf: Command buffer
* @param[in] buf_len: Command length
* @return void
*/
static void gnss_app_send_cmd_by_other_task_int(int8_t* buf, int32_t buf_len)
{
    int ret_len = 0;
    while (1) {
        ret_len += gnss_send_command(buf + ret_len, buf_len - ret_len);
        if (ret_len == buf_len) {
            break;
        }
    }
    xEventGroupSetBits(gnss_task_cntx.gnss_event_group, GNSS_WAIT_SEND_BIT);
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
            return;
        case GNSS_NOTIFICATION_TYPE_DEBUG_INFO:

            return;
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
	GNSSLOGD("gnss_timer_expiry_notify\n");
    xQueueSendFromISR(gnss_task_cntx.gnss_task_queue, ( void * ) &gnss_message, 0);
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
    GNSSLOGD("gnss_msg_handler, message id:%d\n", message->message_id);
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
                gnss_app_send_cmd((int8_t*) "PMTK663", strlen("PMTK663"));//query aiding info status
                gnss_app_send_cmd((int8_t*) "PMTK353,1,0,0,0,0", strlen("PMTK353,1,0,0,0,0"));//set chip to GNSS only mode
                if (g_gnss_location_context.mode != GNSS_LOCATION_MODE_LLE) {
                    gnss_app_send_cmd((int8_t*) "PMTK225,0,0,0", strlen("PMTK225,0,0,0"));//disable lle
                } else {
                    //enable lle, hard code for parameter
                    gnss_app_send_cmd((int8_t*) "PMTK225,2,60000,240000,60000,240000", strlen("PMTK225,1,60000,240000,60000,240000"));
                }
            }
            break;
        case GNSS_ENUM_READY_TO_READ:
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
            gnss_app_send_cmd_by_other_task_int((int8_t*)message->param2, (int32_t)message->param1);
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
}


/**
* @brief GNSS main loop.
* @return void
*/
static void gnss_task_main()
{
    gnss_message_struct_t queue_item;
    GNSSLOGD("gnss_task_main\n");
    //gnss_task_cntx.gnss_event_group = xEventGroupCreate();
    //xEventGroupClearBits(gnss_task_cntx.gnss_event_group, GNSS_WAIT_SEND_BIT);
    
    g_gnss_location_context.state = GNSS_LOCATION_STATE_INIT;
    while (1) {
        if (xQueueReceive(gnss_task_cntx.gnss_task_queue, &queue_item, 10)) {
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
    while (1){
        GNSSLOGD("waiting stats: %d\n", g_gnss_location_context.state);
        if (g_gnss_location_context.state == GNSS_LOCATION_STATE_INIT)
            break;
        vTaskDelay(10);
    }
    return task_handler;
}

void gnss_demo_app_config(int32_t periodic, gnss_location_handle handle)
{
    // send config msg to gnss task.
    GNSSLOGD("gnss_demo_app_config\n");
    gnss_message_struct_t gnss_message;
    gnss_message.message_id = GNSS_ENUM_CONFIG_REQ;
    gnss_message.param1 = periodic;
    gnss_message.param2 = (void*) handle;
    xQueueSend(gnss_task_cntx.gnss_task_queue, &gnss_message, 0);
}

void gnss_demo_app_start()
{
    // send power gnss msg to gnss task.
    GNSSLOGD("gnss_demo_app_start\n");
    gnss_message_struct_t gnss_message;
    gnss_message.message_id = GNSS_ENUM_POWER_ON_REQ;
    xQueueSend(gnss_task_cntx.gnss_task_queue, &gnss_message, 0);
}
 
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

void gnss_demo_app_destroy(TaskHandle_t task_handler)
{
    GNSSLOGD("gnss_demo_app_destroy\n");
	gnss_timer_deinit();
	vQueueDelete(gnss_task_cntx.gnss_task_queue);
	gnss_task_cntx.gnss_task_queue = 0;
    vTaskDelete(task_handler);
}

void gnss_demo_app_send_cmd(int8_t* buf, int32_t buf_len)
{
	gnss_app_send_cmd_by_other_task(buf, buf_len);
}

