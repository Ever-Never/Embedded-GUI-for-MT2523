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

#ifndef __GNSS_DEMO_H__
#define __GNSS_DEMO_H__

#include "FreeRTOS.h"
#include "task.h"

#define GNSS_TASK_NAME "gnss_t"
#define GNSS_TASK_STACK_SIZE 2500 //should be fine tune
#define GNSS_TASK_PRIORITY 3 //should be arrange by scenario


#define GNSS_MAX_GPGGA_SENTENCE_LENGTH 100
#define GNSS_MAX_GPGSA_SENTENCE_LENGTH 80
#define GNSS_MAX_GPRMC_SENTENCE_LENGTH 100
#define GNSS_MAX_GPVTG_SENTENCE_LENGTH 80
#define GNSS_MAX_GPGSV_SENTENCE_LENGTH 400
#define GNSS_MAX_GLGSV_SENTENCE_LENGTH 400
#define GNSS_MAX_BDGSV_SENTENCE_LENGTH 400
#define GNSS_MAX_BDGSA_SENTENCE_LENGTH 80
#define GNSS_MAX_GLGSA_SENTENCE_LENGTH 80
#define GNSS_MAX_GPACC_SENTENCE_LENGHT 80

#define GNSS_MAX_COMMAND_LENGTH 400

typedef enum gnss_message_id {
    GNSS_ENUM_POWER_ON_REQ,
    GNSS_ENUM_POWER_OFF_REQ,
    GNSS_ENUM_CONFIG_REQ,

    GNSS_ENUM_POWER_ON_CNF,
    GNSS_ENUM_POWER_OFF_CNF,
    GNSS_ENUM_READY_TO_READ,
    GNSS_ENUM_READY_TO_WRITE,

    GNSS_ENUM_SEND_COMMAND,

    GNSS_ENUM_TIME_EXPIRY
} gnss_message_id_enum;

typedef struct gnss_message_struct {
    int message_id;
    int param1;
    void* param2;
} gnss_message_struct_t;

typedef struct gnss_sentence_info {
    uint8_t  GPGGA[GNSS_MAX_GPGGA_SENTENCE_LENGTH+1];     /* GGA data */
    uint8_t  GPGSA[GNSS_MAX_GPGSA_SENTENCE_LENGTH+1];     /* GSA data */
    uint8_t  GPRMC[GNSS_MAX_GPRMC_SENTENCE_LENGTH+1];     /* RMC data */
    uint8_t  GPVTG[GNSS_MAX_GPVTG_SENTENCE_LENGTH+1];     /* VTG data */
    uint8_t  GPGSV[GNSS_MAX_GPGSV_SENTENCE_LENGTH+1];     /* GSV data */
    uint8_t  GLGSV[GNSS_MAX_GLGSV_SENTENCE_LENGTH+1];     /* GLONASS GSV data */
    uint8_t  GLGSA[GNSS_MAX_GLGSA_SENTENCE_LENGTH+1];     /* GLONASS GSA data */
    uint8_t  BDGSV[GNSS_MAX_BDGSV_SENTENCE_LENGTH+1];     /* BD GSV data */
    uint8_t  BDGSA[GNSS_MAX_BDGSA_SENTENCE_LENGTH+1];     /* BD GSA data */
    uint8_t  GPACC[GNSS_MAX_GPACC_SENTENCE_LENGHT+1];
} gnss_sentence_info_t;

typedef struct gnss_location_struct {
    int8_t latitude[15];
    int8_t longitude[15];
    int32_t accuracy;
    gnss_sentence_info_t nmea_sentence;
} gnss_location_struct_t;

typedef enum {
    GNSS_LOCATION_ERROR_TYPE_POWER_ON_FAIL,
    GNSS_LOCATION_ERROR_TYPE_POWER_OFF_FAIL,
    GNSS_LOCATION_ERROR_TYPE_ERROR
} gnss_location_error_type_t;

typedef enum {
    GNSS_LOCATION_HANDLE_TYPE_ERROR, // param is gnss_location_error_type_t
    GNSS_LOCATION_HANDLE_TYPE_DATA, // param is gnss_location_struct_t
} gnss_location_handle_type_t;

typedef void (*gnss_location_handle) (gnss_location_handle_type_t type, void* param);

extern void gnss_demo_app_send_cmd(int8_t* buf, int32_t buf_len);

extern void gnss_demo_app_config(int32_t periodic, gnss_location_handle handle);

extern TaskHandle_t gnss_demo_app_create(void);

extern void gnss_demo_app_start(void);

extern void gnss_demo_app_stop(void);

extern void gnss_demo_app_destroy(TaskHandle_t task_handler);

#endif

