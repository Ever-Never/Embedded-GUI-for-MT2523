
/* Copyright Statement:
*
* (C) 2005-2017  MediaTek Inc. All rights reserved.
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


#ifndef __WIFI_FIRMWARE_TASK_H__
#define __WIFI_FIRMWARE_TASK_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <stdbool.h>

#include "wifi_api.h"

/** stack depth 2560 is from BORA MT7687 N9 firmware */
#define WIFI_FIRMWARE_TASK_STACK_DEPTH (2560)

/** heuristics for now */
#define WIIF_FIRMWAREI_TASK_PRIORITY   (6)

/** heuristics for now */
#define WIIF_FIRMWAREI_TASK_ID          "wfw"


/** queue length 16 is from BORA MT7687 N9 firmware */
#define WIFI_FIRMWARE_QUEUE_LENGTH     (16)


/**
 * initialize firmware before task starts
 */
void wifi_firmware_init(void *config, void *config_ext, void * sys_config);


/**
 * this function is exported to enable manual calling.
 *
 * @param arg the argument to the task.
 */
void wifi_firmware_task(void *arg);

void wifi_firmware_set_opmode(void);
void wifi_firmware_reset_opmode(void);
bool  wifi_firmware_ready();


/**
 * creates the Wi-Fi firmware task.
 *
 * @retval 0 the task was created successfully.
 * @retval 1 failed to create the task.
 */
uint8_t wifi_firmware_task_start(void *param); //Castro+


void fw_get_user_config(wifi_config_t ** pt_user_config,wifi_config_ext_t **pt_user_config_ex);

/**
 * simulate an interrupt request
 */
void wifi_firmware_sim_ist(void);
#ifdef KEIL_COMPILE
#ifndef ssize_t
typedef size_t ssize_t;
#endif
#endif
void wifi_generic_timer_callback (void *tmr);
void wifi_generic_inband_callback(uint32_t data);
int wifi_firmware_inband_queue_query(
    unsigned char   cmd_id,
    unsigned char   *buf,
    ssize_t         len,
    unsigned char   *ret_buf,
    ssize_t         out_len,
    unsigned char   is_from_isr,
    unsigned char   is_ext_cmd);

int wifi_firmware_inband_queue_exec(
    unsigned char   cmd_id,
    unsigned char   *buf,
    ssize_t         len,
    unsigned char   is_from_isr,
    unsigned char   is_ext_cmd);
 
/*is_query = 2 ==> Tell N9 need tx status*/
int wifi_firmware_inband_query_tx_frame(
    unsigned char   cmd_id,
    unsigned char   *buf,
    ssize_t         len,
    unsigned char   is_query,
    unsigned char   is_ext_cmd);
#ifdef __cplusplus
}
#endif
#endif /* __WIFI_FIRMWARE_TASK_H__ */