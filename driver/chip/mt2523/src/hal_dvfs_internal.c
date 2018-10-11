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

#include "hal_dvfs_internal.h"

#ifdef HAL_DVFS_MODULE_ENABLED

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "hal_log.h"
#include "hal_nvic_internal.h"
#include "memory_attribute.h"

static dvfs_domain_t *dvfs_domain_head;

ATTR_ZIDATA_IN_TCM bool dvfs_switched_to_privileged;

dvfs_notification_t *dvfs_notify_is_valid(dvfs_domain_t *domain, unsigned int cur_opp,
        unsigned int next_opp)
{
    dvfs_opp_module_t *module;
    dvfs_notification_t *notification;

    if (domain->opp.notification_num) {
        for (module = NULL;;) {
            module = domain->opp.ops.get_next_module(domain, module);
            if (!module) {
                break;
            }
            if (module->frequency[cur_opp] != module->frequency[next_opp]) {
                for (notification = module->notification; ; notification = notification->next) {
                    if (!notification) {
                        break;
                    }
                    if (notification->ops.valid) {
                        if (!notification->ops.valid(domain->opp.voltage[next_opp], module->frequency[next_opp])) {
                            return notification;
                        }
                    }
                }
            }
        }
    }
    return NULL;
}

void dvfs_notify_prepare(dvfs_domain_t *domain, unsigned int cur_opp, unsigned int next_opp)
{
    dvfs_opp_module_t *module;
    dvfs_notification_t *notification;

    if (domain->opp.notification_num) {
        for (module = NULL;;) {
            module = domain->opp.ops.get_next_module(domain, module);
            if (!module) {
                break;
            }
            if (module->frequency[cur_opp] != module->frequency[next_opp]) {
                for (notification = module->notification; ; notification = notification->next) {
                    if (!notification) {
                        break;
                    }
                    if (notification->ops.prepare) {
                        notification->ops.prepare(domain->opp.voltage[next_opp], module->frequency[next_opp]);
                    }
                }
            }
        }
    }
}

int dvfs_query_frequency(uint32_t freq, const uint32_t *frequency, uint32_t num)
{
    unsigned int opp;

    for (opp = 0; opp < num; opp++) {
        if (freq == frequency[opp]) {
            return opp;
        } else if (freq > frequency[opp]) {
            break;
        }
    }

    return DVFS_MODE_NONE;
}

dvfs_domain_t *_dvfs_query_domain(const char *name)
{
    dvfs_domain_t *p = NULL;

    for (p = dvfs_domain_head; ; p = p->next) {
        if (!p) {
            break;
        }

        if (!strcmp(p->name, name)) {
            break;
        }
    }

    return p;
}

dvfs_opp_module_t *_dvfs_map_opp_module(dvfs_domain_t *domain, const char *name)
{
    dvfs_opp_module_t *module;

    for (module = NULL;;) {
        module = domain->opp.ops.get_next_module(domain, module);

        if (!module) {
            break;
        }
        if (!strcmp(module->name, name)) {
            break;
        }
    }

    return module;
}

void dvfs_register_domain(dvfs_domain_t *domain)
{
    dvfs_domain_t *p;

    assert(domain);

    if (!dvfs_domain_head) {
        dvfs_domain_head = domain;
    } else {
        for (p = dvfs_domain_head; ; p = p->next) {
            if (!p) {
                return;
            }

            if (!strcmp(p->name, domain->name)) {
                log_hal_info("domain=%s already exists\r\n", domain->name);
                return;
            }

            if (!p->next) {
                p->next = domain;
                return;
            }
        }
    }
}

void dvfs_register_notification(dvfs_notification_t *node)
{
    uint32_t irq_flag;
    dvfs_notification_t *head;

    assert(node);
    assert(node->domain);
    assert(node->module);
    assert(node->addressee);
    assert(!node->prev);
    assert(!node->next);

    irq_flag = save_and_set_interrupt_mask();

    node->dm = _dvfs_query_domain(node->domain);
    if (!node->dm) {
        goto _exit;
    }

    node->mod = _dvfs_map_opp_module(node->dm, node->module);
    assert(node->mod);

    node->prev = NULL;
    node->next = NULL;

    for (head = node->mod->notification; ; head = head->next) {
        if (!head) {
            node->mod->notification = node;
            break;
        }
        if (!head->next) {
            head->next = node;
            node->prev = head;
            break;
        }
    }

    node->dm->opp.notification_num++;

_exit:
    restore_interrupt_mask(irq_flag);
}

void dvfs_deregister_notification(dvfs_notification_t *node)
{
    uint32_t irq_flag;

    assert(node);

    if (!node->dm) {
        return;
    }

    irq_flag = save_and_set_interrupt_mask();

    if (node->prev) {
        node->prev->next = node->next;
    }

    if (node->next) {
        node->next->prev = node->prev;
    }

    if (node->mod->notification == node) {
        node->mod->notification = node->next;
    }

    node->dm->opp.notification_num--;

    node->prev = NULL;
    node->next = NULL;

    restore_interrupt_mask(irq_flag);
}

void dvfs_lock(dvfs_lock_t *lock)
{
    uint32_t irq_flag;

    assert(lock);
    assert(lock->domain);
    assert(lock->addressee);

    irq_flag = save_and_set_interrupt_mask();

    if (!lock->dm) {
        lock->dm = _dvfs_query_domain(lock->domain);
        if (!lock->dm) {
            goto _exit;
        }
    }

    if (lock->count) {
        lock->count++;
    } else {
        assert(!lock->prev);
        assert(!lock->next);
        lock->count = 1;
        if (!lock->dm->lock_head) {
            assert(!lock->dm->lock_end);
            lock->dm->lock_head = lock;
            lock->dm->lock_end = lock;
        } else {
            lock->dm->lock_end->next = lock;
            lock->prev = lock->dm->lock_end;
            lock->dm->lock_end = lock;
        }
    }

_exit:
    restore_interrupt_mask(irq_flag);
}

void dvfs_unlock(dvfs_lock_t *lock)
{
    uint32_t irq_flag;

    assert(lock);

    if (!lock->dm) {
        return;
    }

    irq_flag = save_and_set_interrupt_mask();

    lock->count--;

    if (!lock->count) {
        if (lock->prev) {
            lock->prev->next = lock->next;
        }

        if (lock->next) {
            lock->next->prev = lock->prev;
        }

        if (lock->dm->lock_end == lock) {
            lock->dm->lock_end = lock->prev;
        }

        if (lock->dm->lock_head == lock) {
            lock->dm->lock_head = lock->next;
        }

        lock->prev = NULL;
        lock->next = NULL;
    }

    restore_interrupt_mask(irq_flag);
}

void dvfs_debug_dump(void)
{
    dvfs_domain_t *domain;
    dvfs_opp_module_t *module;
    dvfs_notification_t *notification;
    dvfs_lock_t *lock;
    unsigned int opp;

    for (domain = dvfs_domain_head; ; domain = domain->next) {
        if (!domain) {
            break;
        }

        log_hal_info("==== domain=%s ====\r\n", domain->name);
        log_hal_info("cur_opp=%u, opp_num=%u, module_num=%u, notification_num=%u, lock=%p\r\n",
                     domain->opp.cur_opp, domain->opp.opp_num,
                     domain->opp.module_num, domain->opp.notification_num, domain->lock_head);
        for (opp = 0; opp < domain->opp.opp_num; opp++) {
            log_hal_info("[%d] frequency=%u, voltage=%u\r\n",
                         opp, domain->opp.frequency[opp], domain->opp.voltage[opp]);
        }

        for (lock = domain->lock_head; ; lock = lock->next) {
            if (!lock) {
                break;
            }
            log_hal_info("[lock %s:%u]\r\n", lock->addressee, lock->count);
        }

        for (module = NULL;;) {
            module = domain->opp.ops.get_next_module(domain, module);

            if (!module) {
                break;
            }

            log_hal_info("== module=%s ==\r\n", module->name);

            for (opp = 0; opp < domain->opp.opp_num; opp++) {
                log_hal_info("[%d] frequency=%u\r\n", opp, module->frequency[opp]);
            }

            for (notification = module->notification; ; notification = notification->next) {
                if (!notification) {
                    break;
                }

                log_hal_info("notificaion=%s\r\n", notification->addressee);
            }
        }

    }
}

#endif /* HAL_DVFS_MODULE_ENABLED */

