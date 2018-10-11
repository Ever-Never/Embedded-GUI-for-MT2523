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

#ifndef _HAL_GPT_INTERNAL_H_
#define _HAL_GPT_INTERNAL_H_

#include "hal_gpt.h"

#ifdef HAL_GPT_MODULE_ENABLED
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "mt7686.h"

#ifdef __cplusplus
extern "C"
{
#endif

//#define GPT_DEBUG_LOG

#define HAL_GPT_MS_PORT             HAL_GPT_5       /* configure HAL_GPT_5 as milliseconds timer */
#define HAL_GPT_US_PORT             HAL_GPT_4       /* configure HAL_GPT_4 as microseconds timer */
#define HAL_GPT_SW_PORT             HAL_GPT_3       /* configure HAL_GPT_3 as software timer */
#define HAL_GPT_SW_NUMBER           32              /* SW GPT support maximal task number*/
#define HAL_GPT_SW_MAGIC            0xdead0000      /* SW GPT magic handle number*/
#define HAL_GPT_SW_HANDLE_MASK      0xffff          /* SW GPT handle mask section*/

#define GPT_COUNT_CLEAR         0x1
#define GPT_COUNT_START         0x1
#define GPT_IRQ_ENABLE          0x1
#define GPT_IRQ_FLAG_STA        0x1
#define GPT_IRQ_FLAG_ACK        0x1
#define GPT_CLOCK_32KHZ         (0x10)
#define GPT_CLOCK_13MHZ         (0x00)
#define GPT_CLOCK_26M_SOURCE    (26000)

#define GPT_REG(_ADDRBASE_) 	(*(volatile unsigned int *)(_ADDRBASE_))
#define GPT_PDN_CLRD2        (0xA2030B20)
#define GPT_PDN_COND2        (0xA2030B00)
#define GPT_PDN_MASK         (1<<17)
#define GPT_MODE_REPEAT_CTR    0x100
#define GPT_MODE_ONE_SHOT_CTR  0
#define GPT_MODE_KEEP_GO_CTR   0x200
#define GPT_MODE_FREE_RUN_CTR  0x300
#define GPT_CLOCK_GATE_CTR     0x10000
#define GPT_CLOCK_UNGATE_CTR   0
/***************************************************************/

#define gpt_sw_delta(data1, data2)  (data1 >= data2) ? (data1 - data2) : (0xffffffff - (data2 - data1) + 1)

typedef enum {
    GPT_MODE_ONE_SHOT = 0,
    GPT_MODE_REPEAT   = 0x100,
    GPT_MODE_KEEP_GO  = 0x200,
    GPT_MODE_FREE_RUN = 0x300
} gpt_mode_type_t;

typedef enum {
    GPT_CLOCK_UNGATE  = 0,
    GPT_CLOCK_GATE    = 0x10000
} gpt_clock_gate_t;

typedef enum {
    GPT_DIVIDE_1  = 0,
    GPT_DIVIDE_2  = 1,
    GPT_DIVIDE_3  = 2,
    GPT_DIVIDE_4  = 3 ,
    GPT_DIVIDE_5  = 4,
    GPT_DIVIDE_6  = 5,
    GPT_DIVIDE_7  = 6,
    GPT_DIVIDE_8  = 7,
    GPT_DIVIDE_9  = 8,
    GPT_DIVIDE_10 = 9,
    GPT_DIVIDE_11 = 10,
    GPT_DIVIDE_12 = 11,
    GPT_DIVIDE_13 = 12,
    GPT_DIVIDE_16 = 13,
    GPT_DIVIDE_32 = 14,
    GPT_DIVIDE_64 = 15
} gpt_clock_divide_t;

typedef struct {
    hal_gpt_callback_t callback;
    void *user_data;
} gpt_callback_context_t;

typedef struct {
    hal_gpt_running_status_t running_status;
    gpt_callback_context_t   callback_context;
    bool                     has_initilized;
    bool                     is_gpt_locked_sleep;
} gpt_context_t;

typedef struct {
    gpt_callback_context_t   callback_context;
    uint32_t                 time_out_ms;
    bool                     is_used;
    bool                     is_running;
} gpt_sw_timer_t;

typedef struct {
    gpt_sw_timer_t  timer[HAL_GPT_SW_NUMBER];
    uint32_t        absolute_time;
    uint32_t        last_absolute_time;
    uint32_t        used_timer_count;
    uint32_t        running_timer_count;
    bool            is_first_init;
    bool            is_start_from_isr;
    bool            is_sw_gpt;
} gpt_sw_context_t;

/********* varible extern *************/
extern GPT_REGISTER_T          *gp_gpt[HAL_GPT_MAX_PORT];
extern gpt_context_t            g_gpt_context[HAL_GPT_MAX_PORT];
extern GPT_REGISTER_GLOABL_T   *gp_gpt_glb;
extern gpt_sw_context_t         gpt_sw_context;
extern uint32_t                 gpt_clock_source;
/***********************/

/******** funtion extern **************/
void gpt_reset_default_timer(GPT_REGISTER_T *gpt);
void gpt_nvic_register(void);
void gpt_delay_time(GPT_REGISTER_T *gpt, uint32_t count);
void  gpt_start_free_run_timer(GPT_REGISTER_T *gpt, uint32_t clock_source, uint32_t divide);
uint32_t gpt_convert_ms_to_32k_count(uint32_t ms);
uint32_t gpt_current_count(GPT_REGISTER_T *gpt);
void gpt_open_clock_source(void);
uint32_t gpt_save_and_mask_interrupt(GPT_REGISTER_T *gpt);
void gpt_restore_interrupt(GPT_REGISTER_T *gpt, uint32_t mask);
void gpt_sw_handler(void *parameter);
uint32_t gpt_sw_get_current_time_ms(GPT_REGISTER_T *gpt);
void gpt_sw_get_minimum_left_time_ms(uint32_t *minimum_time, uint32_t *number);
void gpt_sw_start_timer(void);
uint32_t gpt_sw_get_free_timer(void);

uint32_t gpt_clock_calibrate_us_to_count(uint32_t time_us);
uint32_t gpt_clock_calibrate_count_to_us(uint32_t count);
void gpt_get_lfosc_clock(void);

#ifdef __cplusplus
}
#endif

#endif /*HAL_GPT_MODULE_ENABLED*/

#endif /*_HAL_GPT_INTERNAL_H_*/

