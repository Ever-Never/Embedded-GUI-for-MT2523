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

/**
 * @author  Mediatek
 * @version 1.0
 * @date    2015.10.29
 *
 */

#ifndef __HAL_SYS_TOPSM_H__
#define __HAL_SYS_TOPSM_H__

#include "hal_platform.h"
#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager_platform.h"

typedef enum {
    HAL_SYS_TOPSM_MTCMOS_BTSYS,
    HAL_SYS_TOPSM_MTCMOS_MMSYS,
    HAL_SYS_TOPSM_MTCMOS_MD2G,
    HAL_SYS_TOPSM_MTCMOS_INFRA,
    HAL_SYS_TOPSM_MTCMOS_CMSYS,
    HAL_SYS_TOPSM_MTCMOS_DEBUG
} hal_sys_topsm_mtcmos_enum_t;

/*  SYS_TOPSM_BASE: 0xA20F0000 is used for Larkspur */
#define SYS_TOPSM_RM_PWR_STA                 ((volatile uint32_t*)(SYS_TOPSM_BASE+0x820))  /* Power Status Register  */
#define SYS_TOPSM_RM_TMR_PWR0                ((volatile uint32_t*)(SYS_TOPSM_BASE+0x14))  /* Resource Manager Timer Power Enable Control 0 */
#define SYS_TOPSM_RM_PWR_CON2                ((volatile uint32_t*)(SYS_TOPSM_BASE+0x808))       /* Power Control Register (SW or HW mode) */
#define SYS_TOPSM_RM_PWR_CON1                ((volatile uint32_t*)(SYS_TOPSM_BASE+0x804))       /* Power Control Register (SW or HW mode) */
#define SYS_TOPSM_RM_PWR_CON3                ((volatile uint32_t*)(SYS_TOPSM_BASE+0x80C))       /* Power Control Register (SW or HW mode) */
#define SYS_TOPSM_TOPSM_DBG                  ((volatile uint32_t*)(SYS_TOPSM_BASE+0x50))        /* Sleep Manager Debug Control */

#define SYS_TOPSM_RM_SM_MASK                 ((volatile uint32_t*)(SYS_TOPSM_BASE+0x890))       /* Sleep Manager Enable Control */
#define SYS_TOPSM_RM_SM_PWR                  ((volatile uint32_t*)(SYS_TOPSM_BASE+0x850))       /* Sleep Manager Enable Control */
#define SYS_TOPSM_SW_CLK_FORCE_ON_SET        ((volatile uint32_t*)(SYS_TOPSM_BASE+0xA04))       /* Software Clock Force On Set */
#define SYS_TOPSM_SM_SLV_REQ_STA             ((volatile uint32_t*)(SYS_TOPSM_BASE+0x8E0))       /* Sleep Manager Request Status */
#define SYS_TOPSM_RM_PWR_CON0                ((volatile uint16_t*)(SYS_TOPSM_BASE+0x800))       /* Power Control Register */
#define SYS_TOPSM_DBG_RM_SM_MASK             ((volatile uint32_t*)(SYS_TOPSM_BASE+0x880))       /* Sleep Manager Debug Control Mask */
#define SYS_TOPSM_INDIV_CLK_PROTECT_ACK_MASK ((volatile uint32_t*)(SYS_TOPSM_BASE+0xA34))
#define SYS_TOPSM_CCF_CLK_CON                ((volatile uint32_t*)(SYS_TOPSM_BASE+0x200))

void sys_topsm_mtcmos_control(hal_sys_topsm_mtcmos_enum_t mtcmos, bool config);
void bootrom_system_power_default_setup(void); /*Restore the power domain to boot time*/
void sys_topsm_init(void); /*init sys topsm*/
void sys_topsm_debug_log(hal_sys_topsm_mtcmos_enum_t mtcmos);

#endif /* HAL_SLEEP_MANAGER_ENABLED */
#endif /* __HAL_SYS_TOPSM_H__ */
