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

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "gnss_api.h"
#include "gnss_driver.h"
#include <stdint.h>
#include "gnss_log.h"

#define WAITING_FOR_SEND_CMD_DATA (1 << 1)
#define WAITING_FOR_READ_DATA (1 << 2)
#define GNSS_NMEA_SENTENCE_BUF_LEN 256
#define GNSS_NEMA_TEMP_BUF_LEN 30

log_create_module(GNSS_TAG, PRINT_LEVEL_INFO);

typedef enum{
    GNSS_STATE_POWER_OFF,
    GNSS_STATE_POWER_ON,
    GNSS_STATE_WAITING_POWER_ON
}gnss_state_enum;

typedef enum {
    NMEA_PARSE_STATE_STR,
    NMEA_PARSE_STATE_DATA,
    NMEA_PARSE_STATE_CHECK_SUM1,
    NMEA_PARSE_STATE_CHECK_SUM2,
    NMEA_PARSE_STATE_END_CR,
    NMEA_PARSE_STATE_END_LF
} nmea_parse_state;

static struct {
    uint8_t port;
    gnss_callback_t callback_func;
    gnss_state_enum state;
} g_gnss_context;


void gnss_uart_handle(gnss_uart_callback_type_t type);

void gnss_init(gnss_callback_t callback_function)
{
    if (callback_function == NULL){
        return;
    }
    g_gnss_context.port = gnss_driver_init(gnss_uart_handle);
    g_gnss_context.callback_func = callback_function;
    g_gnss_context.state = GNSS_STATE_POWER_OFF;
}

void gnss_power_on()
{
    if (g_gnss_context.callback_func == NULL) {
        GNSSLOGE("gnss callback is null");
        return;
    }
    // entry critical area,no problem, because of state only change from GNSS_STATE_WAITING_POWER_ON to GNSS_STATE_POWER_ON when interrupt comes.
    if (g_gnss_context.state == GNSS_STATE_POWER_ON) {
        GNSSLOGW("GNSS already power on!\n\r");
        // need to call back??? NO, to avoid potential problem
    } else if (g_gnss_context.state == GNSS_STATE_WAITING_POWER_ON) {
        GNSSLOGW("GNSS already power on!\n\r");
        // just keep waiting
    } else {
        g_gnss_context.state = GNSS_STATE_WAITING_POWER_ON;
        gnss_driver_power_on();
        GNSSLOGI("GNSS try to power on.\n\r");
    }
}

void gnss_power_off (void)
{
    bool is_power_off = true;
    if (g_gnss_context.callback_func == NULL) {
        GNSSLOGE("gnss callback is null");
        return;
    }
    if (g_gnss_context.state == GNSS_STATE_POWER_OFF) {
        GNSSLOGW("GNSS already power off!\n");
        return;
    } else {
        g_gnss_context.state = GNSS_STATE_POWER_OFF;
        gnss_driver_power_off();
        GNSSLOGI("power off GNSS.\n");
    }
    g_gnss_context.callback_func(GNSS_NOTIFICATION_TYPE_POWER_OFF_CNF, &is_power_off);
}

int32_t gnss_send_command (int8_t *cmd, int32_t cmd_len)
{
    int32_t len = 0;
    if (g_gnss_context.state == GNSS_STATE_POWER_ON) {
        len = gnss_driver_uart_write(g_gnss_context.port , cmd, cmd_len);
        //need to output log or not???
        GNSSLOGD("send cmd len:%d", len);
    }
    return len;
}

void gnss_uart_handle(gnss_uart_callback_type_t type)
{

    if (g_gnss_context.state == GNSS_STATE_POWER_OFF) {
        // should not happens, error case.
        //GNSSLOGE("get uart data when gnss already power off");
        return;
    }
    switch (type) {
        case GNSS_UART_CALLBACK_TYPE_CAN_WRITE:
        {
            if (g_gnss_context.callback_func != NULL) {
                g_gnss_context.callback_func(GNSS_NOTIFICATION_TYPE_WRITE, NULL);
            }
            break;
        }
        case GNSS_UART_CALLBACK_TYPE_CAN_READ:
        {
            // assum that: when GNSS power on, GNSS chip will send back a dummy package to confirm.
            // may be, need to check power on result in this case.
            if (g_gnss_context.state == GNSS_STATE_WAITING_POWER_ON) {
                g_gnss_context.callback_func(GNSS_NOTIFICATION_TYPE_POWER_ON_CNF, NULL);
                g_gnss_context.state = GNSS_STATE_POWER_ON;
            }
            if (g_gnss_context.callback_func != NULL) {
                g_gnss_context.callback_func(GNSS_NOTIFICATION_TYPE_READ, NULL);
            }
            break;
        }
        case GNSS_UART_CALLBACK_TYPE_WAKEUP:
            if (g_gnss_context.callback_func != NULL) {
                g_gnss_context.callback_func(GNSS_NOTIFICATION_TYPE_HOST_WAKEUP, NULL);
            }
            break;
        default:
            break;
    }
}


static int32_t gnss_nmea_checksum(int8_t *nmea_buf, int32_t buf_len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t i;
    uint8_t  chksum, chksum2;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (buf_len < 6) {
        return false;
    }
    chksum = nmea_buf[1];
    chksum2 = 0;
    for (i = 2; i < buf_len - 2; i++) {
        if (nmea_buf[i] != '*') {
            chksum ^= nmea_buf[i];
            /* Check if contain invalid character*/
            if (nmea_buf[i] & 0x80) {
                return false;
            }
        } else {
            if (nmea_buf[i + 1] >= 'A') {
                chksum2 = (nmea_buf[i+1]-'A'+10)<<4;
            } else {
                chksum2 = (nmea_buf[i+1]-'0')<<4;
            }
            if (nmea_buf[i + 2] >= 'A') {
                chksum2 += nmea_buf[i+2]-'A'+10;
            } else {
                chksum2 += nmea_buf[i+2]-'0';
            }
            break;
        }
    }
    /* if not found character '*'   */
    if (i == (buf_len - 2)) {
        return false;
    }
    if (chksum == chksum2) {
        return true;
    } else {
        return false;
    }
}

int32_t gnss_read_sentence (int8_t *data_buffer, int32_t length)
{
    static int8_t nmea_sentence[GNSS_NMEA_SENTENCE_BUF_LEN + 1];
    static int32_t nmea_len;
    static nmea_parse_state state;
    static int8_t temp_buf[GNSS_NEMA_TEMP_BUF_LEN + 1];
    static int32_t buf_len;
    static int32_t i;
    gnss_debug_data_t debug_info;
    //GNSSLOGD("gnss_read_sentence\n");
    if (data_buffer == NULL || length <= 0) {
        GNSSLOGE("Please give right parameter\n");
        return 0;
    }

    do {
        if (g_gnss_context.state == GNSS_STATE_POWER_ON) {
            if (i == 0) {
                buf_len = gnss_driver_uart_read(g_gnss_context.port, temp_buf, GNSS_NEMA_TEMP_BUF_LEN);
                temp_buf[buf_len] = '\0';
                debug_info.length = buf_len;
                debug_info.debug_data = temp_buf;
                g_gnss_context.callback_func(GNSS_NOTIFICATION_TYPE_DEBUG_INFO, (void*)&debug_info);
            }
        }
        else {
            GNSSLOGE("Pelease help to power on GNSS first\n");
            return 0;
        }

        while (i < buf_len) {
            //GNSSLOGD("state:%d, character:%c\n", state, temp_buf[i]);
            switch (state) {
                case NMEA_PARSE_STATE_STR:
                    if (temp_buf[i] == '$') {
                        state = NMEA_PARSE_STATE_DATA;
                        nmea_sentence[0] = '$';
                        nmea_len = 1;
                    }
                    break;
                case NMEA_PARSE_STATE_DATA:
                    if (temp_buf[i] == '$') {
                        state = NMEA_PARSE_STATE_DATA;
                        nmea_sentence[0] = '$';
                        nmea_len = 1;
                        break;
                    } else if (temp_buf[i] == '*') {
                        state = NMEA_PARSE_STATE_CHECK_SUM1;
                        nmea_sentence[nmea_len] = temp_buf[i];
                        nmea_len++;
                        break;
                    }
                    if (nmea_len < GNSS_NMEA_SENTENCE_BUF_LEN) {
                        nmea_sentence[nmea_len] = temp_buf[i];
                        nmea_len++;
                    }
                    break;
                case NMEA_PARSE_STATE_CHECK_SUM1:
                    if (nmea_len < GNSS_NMEA_SENTENCE_BUF_LEN) {
                        state = NMEA_PARSE_STATE_CHECK_SUM2;
                        nmea_sentence[nmea_len] = temp_buf[i];
                        nmea_len++;
                    } else {
                        state = NMEA_PARSE_STATE_STR;
                    }
                    break;
                case NMEA_PARSE_STATE_CHECK_SUM2:
                    if (nmea_len < GNSS_NMEA_SENTENCE_BUF_LEN) {
                        state = NMEA_PARSE_STATE_END_CR;
                        nmea_sentence[nmea_len] = temp_buf[i];
                        nmea_len++;
                    } else {
                        state = NMEA_PARSE_STATE_STR;
                    }
                    break;
                case NMEA_PARSE_STATE_END_CR:
                    if (nmea_len < GNSS_NMEA_SENTENCE_BUF_LEN && temp_buf[i] == 0x0D) {
                        state = NMEA_PARSE_STATE_END_LF;
                        nmea_sentence[nmea_len] = temp_buf[i];
                        nmea_len++;
                    } else {
                        state = NMEA_PARSE_STATE_STR;
                    }
                    break;
                case NMEA_PARSE_STATE_END_LF:
                    state = NMEA_PARSE_STATE_STR;
                    if (nmea_len < GNSS_NMEA_SENTENCE_BUF_LEN && temp_buf[i] == 0x0A) {
                        nmea_sentence[nmea_len] = temp_buf[i];
                        nmea_len++;
                        nmea_sentence[nmea_len] = 0;
                        if (gnss_nmea_checksum(nmea_sentence, nmea_len)) {
                            //gnss_nmea_data_process(nmea_sentence, nmea_len);
                            //GNSSLOGD("nmea sentence:%d", nmea_len);
                            i++;
                            if (length > nmea_len) {
                                memcpy(data_buffer, nmea_sentence, nmea_len);
                                return nmea_len;
                            } else {
                                GNSSLOGE("nmea recieve data buf is not enough\n");
                                memcpy(data_buffer, nmea_sentence, length);
                                return length;
                            }
                        } else {
                            // data missing or shifting.
                            GNSSLOGE("nmea data broken:%s\n", nmea_sentence);
                        }
                    } else {
                        //buf is not enough.
                        if (nmea_len >= GNSS_NMEA_SENTENCE_BUF_LEN) {
                            GNSSLOGE("nmea sentence buf is not enough\n");
                        } else {
                            GNSSLOGE("end flag is not 0X0A\n");
                        }
                    }
                    break;
            }
            i++;
        }
        i = 0;
    }
    while (buf_len);
    return 0;
}

