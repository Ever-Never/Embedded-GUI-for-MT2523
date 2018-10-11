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

#include "hal_platform.h"

#ifndef __HAL_SLEEP_MANAGER_PLATFORM_H__
#define __HAL_SLEEP_MANAGER_PLATFORM_H__

#ifdef HAL_SLEEP_MANAGER_ENABLED

typedef enum {
    SLEEP_LOCK_DMA              = 0,
    SLEEP_LOCK_AUDIO_TOP        = 1,
    SLEEP_LOCK_ASYS_TOP         = 2,
    SLEEP_LOCK_SPI_MASTER       = 3,
    SLEEP_LOCK_SPI_SLAVE        = 4,
    SLEEP_LOCK_UART0            = 5,
    SLEEP_LOCK_UART1            = 6,
    SLEEP_LOCK_UART2            = 7,
    SLEEP_LOCK_I2C0             = 8,
    SLEEP_LOCK_I2C1             = 9,
    SLEEP_LOCK_AUDIO_TOP_AHB    = 10,
    SLEEP_LOCK_ASYS_TOP_AHB     = 11,
    SLEEP_LOCK_SDIO_MST         = 12,
    SLEEP_LOCK_SDIO_SLV         = 13,
    SLEEP_LOCK_USER_START_ID    = 14,
    SLEEP_LOCK_INVALID_ID       = 0xFF
} sleep_management_lock_request_t;

typedef enum {
    SLEEP_BACKUP_RESTORE_FLASH          = 0,
    SLEEP_BACKUP_RESTORE_AUDIO_TOP      = 1,
    SLEEP_BACKUP_RESTORE_ASYS_TOP       = 2,
    SLEEP_BACKUP_RESTORE_SPI_MASTER     = 3,
    SLEEP_BACKUP_RESTORE_SPI_SLAVE      = 4,
    SLEEP_BACKUP_RESTORE_UART           = 5,
    SLEEP_BACKUP_RESTORE_I2C            = 6,
    SLEEP_BACKUP_RESTORE_PWM            = 7,
    SLEEP_BACKUP_RESTORE_AUDIO_TOP_AHB  = 8,
    SLEEP_BACKUP_RESTORE_ASYS_TOP_AHB   = 9,
    SLEEP_BACKUP_RESTORE_SDIO_MST       = 10,
    SLEEP_BACKUP_RESTORE_SD_CARD        = 11,
    SLEEP_BACKUP_RESTORE_UART_RX_EINT   = 12,
    SLEEP_BACKUP_RESTORE_CONNNSYS       = 13,
    SLEEP_BACKUP_RESTORE_AUXADC         = 14, 
    SLEEP_BACKUP_RESTORE_MODULE_MAX     = 15,
    SLEEP_BACKUP_RESTORE_USER           = 16
} sleep_management_backup_restore_module_t;

#define SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX 8
#define DEEP_SLEEP_GPT HAL_GPT_0

#endif /* HAL_SLEEP_MANAGER_ENABLED */
#endif

