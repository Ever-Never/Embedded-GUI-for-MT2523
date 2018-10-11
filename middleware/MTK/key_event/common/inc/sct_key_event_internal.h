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
#ifndef __SCT_KEY_EVENT_INTERNAL_H__
#define __SCT_KEY_EVENT_INTERNAL_H__

#ifdef MTK_KEYPAD_ENABLE
#include "sct_keypad.h"
#include "keypad_custom.h"
#endif

#ifdef MTK_EINT_KEY_ENABLE
#include "eint_key_custom.h"
#include "sct_eint_key.h"
#endif

#include "sct_key_event.h"
#include "sct_key_cfw.h"

#ifdef __CFW_CONFIG_SUPPORT__
#include "cfw_define.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define SCT_KEY_POWERKEY_NUMBER     (1)

#define SCT_KEY_KEYPAD_NUMBER       (32)

#ifdef MTK_EINT_KEY_ENABLE
#define SCT_EINT_KEY_NUMBER         BSP_EINT_KEY_NUMBER
#else 
#define SCT_EINT_KEY_NUMBER         0
#endif

#define SCT_KEY_STATE_SIZE          (SCT_KEY_KEYPAD_NUMBER + SCT_KEY_POWERKEY_NUMBER + SCT_EINT_KEY_NUMBER)

#ifndef DEVICE_KEY_NONE
#define DEVICE_KEY_NONE     0xfe
#endif

typedef enum {
    SCT_KEY_DRIVEN_RELEASE        = 0,       
    SCT_KEY_DRIVEN_PRESS          = 1,       
    SCT_KEY_DRIVEN_LONG_PRESS     = 2,       
    SCT_KEY_DRIVEN_REPEAT         = 3,        
    SCT_KEY_DRIVEN_PMU_LONG_PRESS = 4           
} sct_key_driven_t;

typedef struct {
    sct_key_event_t   sct_state[SCT_KEY_STATE_SIZE];
    sct_key_driven_t  key_state[SCT_KEY_STATE_SIZE];
    uint32_t          time_stamp[SCT_KEY_STATE_SIZE];
    uint32_t          timer_handle[SCT_KEY_STATE_SIZE];
    uint32_t          press_time;  
    uint8_t           position[SCT_KEY_STATE_SIZE]; 
    bool              is_mapping_tale;
    uint8_t           key_data;
    bool              has_initilized;
} sct_key_event_state_t;

typedef enum {
    SCT_GET_KEY_DATA_KEYPAD     = 1,
    SCT_GET_KEY_DATA_POWERKEY   = 2,
    SCT_GET_KEY_DATA_EINT       = 3,
    SCT_CALL_CALLBACK           = 4
} sct_key_msg_type_t;

typedef struct {
    char *src_mod;
    sct_key_msg_type_t msg_id;
    uint8_t key_index;
} sct_key_msg_t;


typedef struct {
    sct_key_callback_t callback;
    void *user_data;
} sct_key_event_context_t;



typedef struct {
    sct_key_driven_t   state;       
    uint32_t   key_data;          
    uint32_t   time_stamp;        
} sct_key_mapping_event_t;


/********* varible extern *************/
extern uint32_t sct_key_driven_signal_time;
extern sct_key_event_context_t sct_key_event_context;
extern sct_key_event_state_t   sct_key_event_state;
extern sct_key_event_time_t    sct_key_event_time;

/******** funtion extern **************/
void sct_key_task_init(void);
void sct_key_send_message(sct_key_msg_type_t msg, uint8_t *key_index);

#ifdef __cplusplus
}
#endif

#endif /* __SCT_KEY_EVENT_INTERNAL_H__ */

