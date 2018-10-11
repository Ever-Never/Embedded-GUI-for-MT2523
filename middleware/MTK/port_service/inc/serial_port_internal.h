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

#ifndef __SERIAL_PORT_INTERNAL_H__
#define __SERIAL_PORT_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MTK_PORT_SERVICE_ENABLE

#include <assert.h>

#define SERIAL_PORT_MAGIC_NUM 'M'
#define SERIAL_PORT_MAGIC_NUM_MASK 0xff
#define SERIAL_PORT_MAGIC_NUM_SHIFT 24
#define SERIAL_PORT_STATUS_INVALID 'I'
#define SERIAL_PORT_STATUS_VALID 'V'
#define SERIAL_PORT_STATUS_MARK 0xff
#define SERIAL_PORT_STATUS_SHIFT 16
#define SERIAL_PORT_OPEN_COUNT_MASK 0xff
#define SERIAL_PORT_OPEN_COUNT_SHIFT 8
#define SERIAL_PORT_DEV_MASK 0xff

typedef struct {
    serial_port_dev_setting_t setting[SERIAL_PORT_DEV_MAX];
} serial_port_setting_t;

typedef struct {
    serial_port_status_t (*init)(serial_port_dev_t device, serial_port_open_para_t *para, void *priv_data);
    serial_port_status_t (*control)(serial_port_dev_t device, serial_port_ctrl_cmd_t cmd, serial_port_ctrl_para_t *para);
    serial_port_status_t (*deinit)(serial_port_dev_t device);
    void (*reset_para)(serial_port_setting_t *setting);
} serial_port_operation_t;

serial_port_status_t serial_port_uart_init(serial_port_dev_t device, serial_port_open_para_t *para, void *priv_data);
serial_port_status_t serial_port_uart_control(serial_port_dev_t device, serial_port_ctrl_cmd_t cmd, serial_port_ctrl_para_t *para);
serial_port_status_t serial_port_uart_deinit(serial_port_dev_t device);
void serial_port_uart_reset_para(serial_port_setting_t *setting);
serial_port_status_t serial_port_usb_init(serial_port_dev_t device, serial_port_open_para_t *para, void *priv_data);
serial_port_status_t serial_port_usb_control(serial_port_dev_t device, serial_port_ctrl_cmd_t cmd, serial_port_ctrl_para_t *para);
serial_port_status_t serial_port_usb_deinit(serial_port_dev_t device);

#define SERIAL_PORT_ASSERT() assert(0)

#endif

#ifdef __cplusplus
}
#endif

#endif

