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

#include "hal_dvfs.h"

#ifdef HAL_DVFS_MODULE_ENABLED

#include <assert.h>

#include "hal_dvfs_internal.h"
#include "hal_log.h"
#include "hal_nvic_internal.h"

static dvfs_domain_t *pvcore_domain;

static int dvfs_search_opp(dvfs_domain_t *domain, uint32_t target_freq,
                           hal_dvfs_freq_relation_t relation)
{
    uint32_t opp;

    assert(domain->opp.opp_num >= 1);

    if (target_freq >= domain->opp.frequency[0]) {
        return 0;
    } else if (target_freq <= domain->opp.frequency[domain->opp.opp_num - 1]) {
        return (domain->opp.opp_num - 1);
    }

    for (opp = 0; opp < domain->opp.opp_num; opp++) {
        if (target_freq == domain->opp.frequency[opp]) {
            return opp;
        } else if (target_freq > domain->opp.frequency[opp]) {
            break;
        }
    }

    switch (relation) {
        case HAL_DVFS_FREQ_RELATION_L:
            return opp - 1;
        case HAL_DVFS_FREQ_RELATION_H:
            return opp;
        default:
            return -1;
    }
}

static hal_dvfs_status_t dvfs_get_frequency_list(dvfs_domain_t *domain, const uint32_t **list, uint32_t *list_num)
{
    assert(list);
    assert(list_num);

    if (!domain || !domain->initialized) {
        log_hal_info("[%s] non-init\r\n", __FUNCTION__);
        return HAL_DVFS_STATUS_UNINITIALIZED;
    }

    *list = domain->opp.frequency;
    *list_num = domain->opp.opp_num;

    return HAL_DVFS_STATUS_OK;
}

static uint32_t dvfs_get_frequency(dvfs_domain_t *domain)
{
    if (!domain || !domain->initialized) {
        log_hal_info("[%s] non-init\r\n", __FUNCTION__);
        return 0;
    }

    return domain->opp.frequency[domain->opp.cur_opp];
}

static hal_dvfs_status_t dvfs_target_frequency(dvfs_domain_t *domain, uint32_t target_freq,
        hal_dvfs_freq_relation_t relation)
{
    int idx;
    uint32_t irq_flag;
    unsigned int next_opp, old_opp;
    dvfs_notification_t *notification;
    dvfs_lock_t *lock;
    hal_dvfs_status_t ret = HAL_DVFS_STATUS_OK;

    if (!domain || !domain->initialized) {
        log_hal_info("[%s] non-init\r\n", __FUNCTION__);
        return HAL_DVFS_STATUS_UNINITIALIZED;
    }

    idx = dvfs_search_opp(domain, target_freq, relation);
    if (idx < 0) {
        log_hal_info("[%s] invalid target=%u, relation=%u\r\n", __FUNCTION__,
                     (unsigned int)target_freq, (unsigned int)relation);
        return HAL_DVFS_STATUS_INVALID_PARAM;
    }

    next_opp = (unsigned int)idx;

    irq_flag = save_and_set_interrupt_mask();

    old_opp = domain->opp.cur_opp;

    if (domain->lock_head) {
        ret = HAL_DVFS_STATUS_NOT_PERMITTED;
        goto _exit;
    }

    /* Notify for checking if voltage change is valid or not */
    notification = dvfs_notify_is_valid(domain, old_opp, next_opp);
    if (notification) {
        ret = HAL_DVFS_STATUS_BUSY;
        goto _exit;
    }

    if (domain->opp.ops.valid && !domain->opp.ops.valid(domain, old_opp, next_opp)) {
        ret = HAL_DVFS_STATUS_NOT_PERMITTED;
        goto _exit;
    }

    if (next_opp <= old_opp) {
        /* Voltage level up */
        domain->opp.ops.switch_voltage(domain, old_opp, next_opp);

        /* Switch frequencies */
        domain->opp.ops.switch_frequency(domain, old_opp, next_opp);
    }

    /* Notify for clock source frequency is changing */
    dvfs_notify_prepare(domain, old_opp, next_opp);

    if (next_opp > old_opp) {
        /* Switch frequencies */
        domain->opp.ops.switch_frequency(domain, old_opp, next_opp);

        /* Voltage level down */
        domain->opp.ops.switch_voltage(domain, old_opp, next_opp);
    }

    domain->opp.cur_opp = next_opp;

_exit:
    restore_interrupt_mask(irq_flag);

    if (ret == HAL_DVFS_STATUS_NOT_PERMITTED) {
        log_hal_info("not permitted lock=%p\r\n", domain->lock_head);
        for (lock = domain->lock_head; ; lock = lock->next) {
            if (!lock) {
                break;
            }
            log_hal_info("[lock %s:%u]\r\n", lock->addressee, lock->count);
        }
    } else if (ret == HAL_DVFS_STATUS_BUSY) {
        log_hal_info("busy domain=%s, module=%s, addressee=%s\r\n",
                     notification->domain, notification->module, notification->addressee);
    }
    return ret;
}

hal_dvfs_status_t hal_dvfs_target_cpu_frequency(uint32_t target_freq, hal_dvfs_freq_relation_t relation)
{
    return dvfs_target_frequency(pvcore_domain, target_freq, relation);
}

uint32_t hal_dvfs_get_cpu_frequency(void)
{
    return dvfs_get_frequency(pvcore_domain);
}

hal_dvfs_status_t hal_dvfs_get_cpu_frequency_list(const uint32_t **list, uint32_t *list_num)
{
    return dvfs_get_frequency_list(pvcore_domain, list, list_num);
}

hal_dvfs_status_t hal_dvfs_init(void)
{
    if (!pvcore_domain) {
        pvcore_domain = dvfs_vcore_domain_initialize();
        if (!pvcore_domain) {
            log_hal_info("[%s] vcore initialized failed\r\n", __FUNCTION__);
            return HAL_DVFS_STATUS_ERROR;
        }
    }

    return HAL_DVFS_STATUS_OK;
}

#endif /* HAL_DVFS_MODULE_ENABLED */

