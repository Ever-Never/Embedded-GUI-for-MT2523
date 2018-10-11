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

#ifndef __TASK_DEF_H__
#define __TASK_DEF_H__

#include "FreeRTOSConfig.h"

typedef enum {
    TASK_PRIORITY_IDLE = 0,                                 /* lowest, special for idle task */
    TASK_PRIORITY_SYSLOG,                                   /* special for syslog task */

    /* User task priority begin, please define your task priority at this interval */
    TASK_PRIORITY_LOW,                                      /* low */
    TASK_PRIORITY_BELOW_NORMAL,                             /* below normal */
    TASK_PRIORITY_NORMAL,                                   /* normal */
    TASK_PRIORITY_ABOVE_NORMAL,                             /* above normal */
    TASK_PRIORITY_HIGH,                                     /* high */
    TASK_PRIORITY_SOFT_REALTIME,                            /* soft real time */
    TASK_PRIORITY_HARD_REALTIME,                            /* hard real time */
    /* User task priority end */

    /*Be careful, the max-priority number can not be bigger than configMAX_PRIORITIES - 1, or kernel will crash!!! */
    TASK_PRIORITY_TIMER = configMAX_PRIORITIES - 1,         /* highest, special for timer task to keep time accuracy */
} task_priority_type_t;

/*The following is an example to define the XXXX task.
#define XXXX_TASK_NAME "XXXX"
#define XXXX_TASK_STACKSIZE 512
#define XXXX_TASK_PRIO TASK_PRIORITY_LOW
#define XXXX_QUEUE_LENGTH  16
*/

/* part_1: SDK tasks configure infomation, please don't modify */
/* ctp task definition*/
#define BSP_CTP_TASK_NAME       "CTP"
#define BSP_CTP_TASK_STACKSIZE  (1024*4) /* unit byte!*/
#define BSP_CTP_TASK_PRIO       TASK_PRIORITY_SOFT_REALTIME
#define BSP_CTP_QUEUE_LENGTH    20


/* battery_management */
#define BMT_TASK_NAME "BMT"
#define BMT_TASK_STACKSIZE 4096
#define BMT_TASK_PRIO TASK_PRIORITY_ABOVE_NORMAL
#define BMT_QUEUE_LENGTH  1


/* syslog task definition */
#define SYSLOG_TASK_NAME "SYSLOG"
/*This definition determines whether the port service feature is enabled. If it is not defined, then the port service feature is not supported.*/
#if defined(MTK_PORT_SERVICE_ENABLE)
#define SYSLOG_TASK_STACKSIZE 1024
#else
#define SYSLOG_TASK_STACKSIZE 192
#endif
#define SYSLOG_TASK_PRIO TASK_PRIORITY_SYSLOG
/*MTK_NO_PSRAM_ENABLE: the definition of MTK_NO_PSRAM_ENABLE determines whether the demo uses PSRAM. If it is defined, means no PSRAM in this project.*/
#if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697) || defined(MTK_NO_PSRAM_ENABLE)
#define SYSLOG_QUEUE_LENGTH 8
#elif (PRODUCT_VERSION == 2523)
#define SYSLOG_QUEUE_LENGTH 512
#endif

/* ATCI task definition */
#define ATCI_TASK_NAME              "ATCI"
/*This definition determines whether the demo uses ATCI via Port Service port, such as UART/USB port. If it is not defined, then the demo accesses the ATCI via UART port only.*/
#if defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)
/*This definition determines whether the demo uses audio tuning functions, such as tuning audio performace or tuning gains. If it is not defined, then the demo does not using audio tuning functions.*/
#ifdef MTK_AUDIO_TUNING_ENABLED
#define ATCI_TASK_STACKSIZE ((5000 + 1024)*4) /*unit byte!*/
#else
#define ATCI_TASK_STACKSIZE ((1024 + 1024)*4) /*unit byte!*/
#endif
#else
/*This definition determines whether the demo uses audio tuning functions, such as tuning audio performace or tuning gains. If it is not defined, then the demo does not using audio tuning functions.*/
#ifdef MTK_AUDIO_TUNING_ENABLED
#define ATCI_TASK_STACKSIZE         (5000*4) /*unit byte!*/
#else
#define ATCI_TASK_STACKSIZE         (1024*4) /*unit byte!*/
#endif
#endif
#define ATCI_TASK_PRIO              TASK_PRIORITY_NORMAL

/* Sensor task definition */
#define SENSOR_MGR_TASK_NAME "SensorMgr"
#define SENSOR_MGR_STACKSIZE 2048 /* unit byte!*/
#define SENSOR_MGR_TASK_PRIO TASK_PRIORITY_SOFT_REALTIME

#define SENSOR_FUSION_TASK_NAME "SensorFusion"
#define SENSOR_FUSION_STACKSIZE 6144 /* unit byte!*/
#define SENSOR_FUSION_TASK_PRIO TASK_PRIORITY_NORMAL

/* atci ctp task*/
#define ATCI_CTP_TASK_NAME "CTP_atci"
#define ATCI_CTP_TASK_STACKSIZE (512*4) /* unit byte!*/
#define ATCI_CTP_TASK_PRIO TASK_PRIORITY_NORMAL
#define ATCI_CTP_QUEUE_LENGTH  20


/* atci keypad task*/
#define ATCI_KEYPAD_TASK_NAME "KEYPAD_atci"
#define ATCI_KEYPAD_TASK_STACKSIZE (512*4) /* unit byte!*/
#define ATCI_KEYPAD_TASK_PRIO TASK_PRIORITY_NORMAL
#define ATCI_KEYPAD_QUEUE_LENGTH  100

/* ATCI MSDC task definition */
#define ATCI_MSDC_TASK_NAME              "MSDC_atci"
#define ATCI_MSDC_TASK_STACKSIZE         (200*4) /*unit byte!*/
#define ATCI_MSDC_TASK_PRIO              TASK_PRIORITY_NORMAL

/* ATCI BIO_UI task definition */
#define ATCI_BIO_TASK_NAME              "BIO_atci"
#define ATCI_BIO_TASK_STACKSIZE         4096 /*unit byte!*/
#define ATCI_BIO_TASK_PRIO              TASK_PRIORITY_NORMAL

/* ATCI  task definition */
#define ATCI_BIO_SMT_TASK_NAME              "BIO_SMT_atci"
#define ATCI_BIO_SMT_TASK_STACKSIZE         4096 /*unit byte!*/
#define ATCI_BIO_SMT_TASK_PRIO              TASK_PRIORITY_NORMAL


/* BT task definition */
#define BT_TASK_NAME              "bt_task"
#define BT_TASK_STACKSIZE         (1024*4) /*unit byte*/
#define BT_TASK_PRIORITY          TASK_PRIORITY_HIGH

/* BT codec task definition */
#define BT_CODEC_TASK_NAME       "BT_codec_task"
#define BT_CODEC_TASK_STACKSIZE  (4096)
#define BT_CODEC_TASK_PRIO       TASK_PRIORITY_SOFT_REALTIME

/* Audio manager task definition */
#define AM_TASK_NAME             "AudioManager"
#define AM_TASK_STACKSIZE        (2048)
#define AM_TASK_PRIO             TASK_PRIORITY_SOFT_REALTIME

/* Battery message task definition */
#define BMT_APP_TASK_NAME "battery_message_task"
#define BMT_APP_TASK_STACKSIZE 1024
#define BMT_APP_TASK_PRIO TASK_PRIORITY_NORMAL
#define BMT_APP_QUEUE_LENGTH  5
/* for os utilization task */
/*MTK_OS_CPU_UTILIZATION_ENABLE: This definition determines whether enable CPU utilization profiling feature, if enabled, can use AT command on MT25x3 or CLI command on MT76x7 to do CPU utilization profiling.*/
#if defined(MTK_OS_CPU_UTILIZATION_ENABLE)
#define MTK_OS_CPU_UTILIZATION_TASK_NAME "CPU"
#define MTK_OS_CPU_UTILIZATION_STACKSIZE 768 /*unit byte*/
#define MTK_OS_CPU_UTILIZATION_PRIO      TASK_PRIORITY_SOFT_REALTIME
#endif
/* part_2: Application and customer tasks configure information */
/* currently, only UI task and tasks to show example project function which define in apps/project/src/main.c */

/* for Application task */

/* for MP3 codec task */
#define MP3_CODEC_TASK_NAME       "mp3_codec_task"
#define MP3_CODEC_TASK_STACKSIZE  1024
#define MP3_CODEC_TASK_PRIO       TASK_PRIORITY_SOFT_REALTIME
#define MP3_CODEC_QUEUE_LENGTH 20

#ifdef MTK_USB_DEMO_ENABLED
/* USB */
#define USB_TASK_NAME "USB"
#define USB_TASK_STACKSIZE 4096
#define USB_TASK_PRIO TASK_PRIORITY_HIGH
#define USB_QUEUE_LENGTH  500
#endif

/* for audio codec task */
#define AUDIO_CODEC_TASK_NAME       "audio_codec_task"
#define AUDIO_CODEC_TASK_STACKSIZE  1024
#define AUDIO_CODEC_TASK_PRIO       TASK_PRIORITY_NORMAL
#define AUDIO_CODEC_QUEUE_LENGTH    20


// #define MTK_MINICLI_ENABLE
/*****************************************************************************
 * API Functions
 *****************************************************************************/

void task_def_init(void);

void task_def_create(void);

void task_def_delete_wo_curr_task(void);

void task_def_dump_stack_water_mark(void);

#endif

