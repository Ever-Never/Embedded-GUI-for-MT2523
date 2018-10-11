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

#ifndef __HAL_DVFS_INTERNAL_H__
#define __HAL_DVFS_INTERNAL_H__

#include "hal_platform.h"

#ifdef HAL_DVFS_MODULE_ENABLED

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define DVFS_MODE_NONE (-1)
#define DVFS_NULL_HANDLE 0

typedef unsigned int dvfs_handle;

/**
 * struct dvfs_notification_ops_t - Callbacks for notify dvfs' related operations.
 *
 * valid: Callback to determine if given voltage and clock source frequency
 *        is supported by the callee or not.
 *        voltage : in uV.
 *        frequency : in kHz.
 *        Return true means valid (ie. supported).
 *        Return false means invalid (ie. not supported).
 *
 * prepare: Callback to notify the addressee to prepare for the given voltage and
 *          clock source frequency.
 *          voltage : in uV.
 *          frequency : in kHz.
 */
typedef struct {
    bool (*valid)(uint32_t voltage, uint32_t frequency);
    void (*prepare)(uint32_t voltage, uint32_t frequency);
} dvfs_notification_ops_t;

typedef struct _dvfs_notification_t {
    const char *domain;
    const char *module;
    const char *addressee;
    dvfs_notification_ops_t ops;
    struct _dvfs_domain_t *dm;
    struct _dvfs_opp_module_t *mod;
    struct _dvfs_notification_t *prev;
    struct _dvfs_notification_t *next;
} dvfs_notification_t;

typedef struct _dvfs_opp_module_t {
    const char *name;
    dvfs_notification_t *notification;
    const uint32_t *frequency;
} dvfs_opp_module_t;

typedef struct {
    bool (*valid)(struct _dvfs_domain_t *domain, unsigned int cur_opp, unsigned int next_opp);
    void (*switch_voltage)(struct _dvfs_domain_t *domain, unsigned int cur_opp, unsigned int next_opp);
    void (*switch_frequency)(struct _dvfs_domain_t *domain, unsigned int cur_opp, unsigned int next_opp);
    dvfs_opp_module_t *(*get_next_module)(struct _dvfs_domain_t *domain, dvfs_opp_module_t *module);
} dvfs_opp_ops_t;

typedef struct _dvfs_lock_t {
    const char *domain;
    const char *addressee;
    unsigned int count;
    struct _dvfs_domain_t *dm;
    struct _dvfs_lock_t *prev;
    struct _dvfs_lock_t *next;
} dvfs_lock_t;

typedef struct {
    unsigned int cur_opp;
    uint32_t opp_num;
    unsigned int module_num;
    unsigned int notification_num;
    const uint32_t *voltage;
    const uint32_t *frequency;
    dvfs_opp_ops_t ops;
    void *data;
} dvfs_opp_t;

typedef struct _dvfs_domain_t {
    const char *name;
    bool initialized;
    dvfs_opp_t opp;
    dvfs_lock_t *lock_head;
    dvfs_lock_t *lock_end;
    struct _dvfs_domain_t *next;
} dvfs_domain_t;

dvfs_notification_t *dvfs_notify_is_valid(dvfs_domain_t *domain, unsigned int cur_opp,
        unsigned int next_opp);
void dvfs_notify_prepare(dvfs_domain_t *domain, unsigned int cur_opp, unsigned int next_opp);
int dvfs_query_frequency(uint32_t freq, const uint32_t *frequency, uint32_t num);

void dvfs_register_domain(dvfs_domain_t *domain);

void dvfs_register_notification(dvfs_notification_t *node);
void dvfs_deregister_notification(dvfs_notification_t *node);

void dvfs_lock(dvfs_lock_t *lock);
void dvfs_unlock(dvfs_lock_t *lock);

void dvfs_debug_dump(void);

extern bool dvfs_switched_to_privileged;

#define dvfs_enter_privileged_level() \
    do { \
        register uint32_t control = __get_CONTROL(); \
        CONTROL_Type pControl; \
        *(uint32_t *)&pControl = control; \
        if (pControl.b.SPSEL == 1) { \
            /* Alter MSP as stack pointer. */ \
            dvfs_switched_to_privileged = TRUE; \
            pControl.b.SPSEL = 0; \
            control = *(uint32_t *)&pControl; \
            __ISB(); \
            __DSB(); \
            __set_CONTROL(control); \
            __ISB(); \
            __DSB(); \
        } \
    } while(0)

#define dvfs_exit_privileged_level() \
    do { \
        register uint32_t control = __get_CONTROL(); \
        CONTROL_Type pControl; \
        if (dvfs_switched_to_privileged == TRUE) { \
            *(uint32_t *)&pControl = control; \
            dvfs_switched_to_privileged = FALSE; \
            pControl.b.SPSEL = 1; \
            control = *(uint32_t *)&pControl; \
            __ISB(); \
            __DSB(); \
            __set_CONTROL(control); \
            __ISB(); \
            __DSB(); \
        } \
    } while(0)


dvfs_domain_t *dvfs_vcore_domain_initialize(void);

#endif /* HAL_DVFS_MODULE_ENABLED */

#endif /* __HAL_DVFS_INTERNAL_H__ */
