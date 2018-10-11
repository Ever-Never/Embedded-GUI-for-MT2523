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

#ifdef MTK_PORT_SERVICE_ENABLE

#include "string.h"
#include "serial_port.h"
#include "serial_port_internal.h"
#include "nvdm.h"

#include "syslog.h"
log_create_module(SPP_PORT, PRINT_LEVEL_INFO);

void serial_port_uart_reset_para(serial_port_setting_t *setting)
{
    serial_port_dev_t device;
    serial_port_setting_uart_t *port_setting_uart;

    for (device = SERIAL_PORT_DEV_UART_TYPE_BEGIN; device <= SERIAL_PORT_DEV_UART_TYPE_END; device++) {
        port_setting_uart = (serial_port_setting_uart_t *) & (setting->setting[device]);
        port_setting_uart->baudrate = UART_DEFAULT_BAUDRATE;
    }
}

extern serial_port_operation_t g_serial_port_handlers[];

serial_port_status_t serial_port_config_dump_dev_number(uint32_t *user_count, serial_port_assign_t *port_assign)
{
    uint32_t setting_size;
    serial_port_assign_t serial_port_assign[SERIAL_PORT_DEV_MAX];
    nvdm_status_t status;

    setting_size = sizeof(serial_port_assign_t) * SERIAL_PORT_DEV_MAX;
    status = nvdm_read_data_item(PORT_SETTING_GROUP_NAME, PORT_SETTING_NAME_PORT_ASSIGN, (uint8_t *)serial_port_assign, &setting_size);
    if (status == NVDM_STATUS_ITEM_NOT_FOUND) {
        return SERIAL_PORT_STATUS_FAIL;
    } else if (status != NVDM_STATUS_OK) {
        SERIAL_PORT_ASSERT();
        return SERIAL_PORT_STATUS_FAIL;
    }

    *user_count = setting_size / sizeof(serial_port_assign_t);
    memcpy(port_assign, serial_port_assign, setting_size);

    return SERIAL_PORT_STATUS_OK;
}

serial_port_status_t serial_port_config_read_dev_number(const char *user_name, serial_port_dev_t *device)
{
    uint32_t i, setting_size, setting_count;
    serial_port_assign_t serial_port_assign[SERIAL_PORT_DEV_MAX];
    nvdm_status_t status;

    if ((strlen(user_name) >= SERIAL_PORT_USER_NAME_SIZE) || (device == NULL)) {
        return SERIAL_PORT_STATUS_INVALID_PARAMETER;
    }

    setting_size = sizeof(serial_port_assign_t) * SERIAL_PORT_DEV_MAX;
    status = nvdm_read_data_item(PORT_SETTING_GROUP_NAME, PORT_SETTING_NAME_PORT_ASSIGN, (uint8_t *)serial_port_assign, &setting_size);
    if (status == NVDM_STATUS_ITEM_NOT_FOUND) {
        return SERIAL_PORT_STATUS_FAIL;
    } else if (status != NVDM_STATUS_OK) {
        SERIAL_PORT_ASSERT();
        return SERIAL_PORT_STATUS_FAIL;
    }

    setting_count = setting_size / sizeof(serial_port_assign_t);
    for (i = 0; i < setting_count; i++) {
        if (strcmp(serial_port_assign[i].name, user_name) == 0) {
            break;
        }
    }
    if (i >= setting_count) {
        return SERIAL_PORT_STATUS_FAIL;
    }

    *device = serial_port_assign[i].device;

    return SERIAL_PORT_STATUS_OK;
}

serial_port_status_t serial_port_config_write_dev_number(const char *user_name, serial_port_dev_t device)
{
    bool found;
    uint32_t i, setting_size, setting_count;
    serial_port_assign_t serial_port_assign[SERIAL_PORT_DEV_MAX];
    nvdm_status_t status;

    if ((strlen(user_name) >= SERIAL_PORT_USER_NAME_SIZE) || (device >= SERIAL_PORT_DEV_MAX)) {
        return SERIAL_PORT_STATUS_INVALID_PARAMETER;
    }

    memset(serial_port_assign, 0, (sizeof(serial_port_assign_t)*SERIAL_PORT_DEV_MAX));

    /* check whether we can add more user? */
    setting_size = sizeof(serial_port_assign_t) * SERIAL_PORT_DEV_MAX;
    status = nvdm_read_data_item(PORT_SETTING_GROUP_NAME, PORT_SETTING_NAME_PORT_ASSIGN, (uint8_t *)serial_port_assign, &setting_size);
    if (status == NVDM_STATUS_ITEM_NOT_FOUND) {
        memcpy(serial_port_assign[0].name, user_name, strlen(user_name));
        serial_port_assign[0].device = device;
        nvdm_write_data_item(PORT_SETTING_GROUP_NAME, PORT_SETTING_NAME_PORT_ASSIGN, NVDM_DATA_ITEM_TYPE_RAW_DATA, (const uint8_t *)serial_port_assign, sizeof(serial_port_assign_t));
    } else if (status == NVDM_STATUS_OK) {
        found = false;
        setting_count = setting_size / sizeof(serial_port_assign_t);
        for (i = 0; i < setting_count; i++) {
            if (strcmp(serial_port_assign[i].name, user_name) == 0) {
                found = true;
                break;
            }
        }
        if (found == false) {
            if (setting_count >= SERIAL_PORT_DEV_MAX) {
                return SERIAL_PORT_STATUS_FAIL;
            }
            memcpy(serial_port_assign[i].name, user_name, strlen(user_name));
            serial_port_assign[i].device = device;
            setting_size += sizeof(serial_port_assign_t);
            nvdm_write_data_item(PORT_SETTING_GROUP_NAME, PORT_SETTING_NAME_PORT_ASSIGN, NVDM_DATA_ITEM_TYPE_RAW_DATA, (const uint8_t *)serial_port_assign, setting_size);
        } else {
            serial_port_assign[i].device = device;
            nvdm_write_data_item(PORT_SETTING_GROUP_NAME, PORT_SETTING_NAME_PORT_ASSIGN, NVDM_DATA_ITEM_TYPE_RAW_DATA, (const uint8_t *)serial_port_assign, setting_size);
        }
    } else {
        SERIAL_PORT_ASSERT();
        return SERIAL_PORT_STATUS_FAIL;
    }

    return SERIAL_PORT_STATUS_OK;
}

static void serial_port_config_check_default_setting(serial_port_setting_t *serial_port_setting)
{
    serial_port_type_t dev_type;
    uint32_t setting_size;
    nvdm_status_t status;

    /* check whether we can add more user? */
    setting_size = sizeof(serial_port_setting_t);
    status = nvdm_read_data_item(PORT_SETTING_GROUP_NAME, PORT_SETTING_NAME_PORT_CONFIG, (uint8_t *)serial_port_setting, &setting_size);
    if (status == NVDM_STATUS_ITEM_NOT_FOUND) {
        for (dev_type = SERIAL_PORT_TYPE_BEGIN; dev_type < SERIAL_PORT_TYPE_MAX; dev_type++) {
            if (g_serial_port_handlers[dev_type].reset_para != NULL) {
                g_serial_port_handlers[dev_type].reset_para(serial_port_setting);
            }
        }
        setting_size = sizeof(serial_port_setting_t);
        nvdm_write_data_item(PORT_SETTING_GROUP_NAME, PORT_SETTING_NAME_PORT_CONFIG, NVDM_DATA_ITEM_TYPE_RAW_DATA, (const uint8_t *)serial_port_setting, setting_size);
    } else if (status != NVDM_STATUS_OK) {
        SERIAL_PORT_ASSERT();
    }
}

serial_port_status_t serial_port_config_read_dev_setting(serial_port_dev_t device, serial_port_dev_setting_t *dev_setting)
{
    serial_port_setting_t serial_port_setting;

    if ((device >= SERIAL_PORT_DEV_MAX) || (dev_setting == NULL)) {
        return SERIAL_PORT_STATUS_INVALID_PARAMETER;
    }

    serial_port_config_check_default_setting(&serial_port_setting);

    memcpy(dev_setting, &(serial_port_setting.setting[device]), sizeof(serial_port_dev_setting_t));

    return SERIAL_PORT_STATUS_OK;
}

serial_port_status_t serial_port_config_write_dev_setting(serial_port_dev_t device, serial_port_dev_setting_t *dev_setting)
{
    serial_port_setting_t serial_port_setting;

    if ((device >= SERIAL_PORT_DEV_MAX) || (dev_setting == NULL)) {
        return SERIAL_PORT_STATUS_INVALID_PARAMETER;
    }

    serial_port_config_check_default_setting(&serial_port_setting);

    memcpy(&(serial_port_setting.setting[device]), dev_setting, sizeof(serial_port_dev_setting_t));
    nvdm_write_data_item(PORT_SETTING_GROUP_NAME, PORT_SETTING_NAME_PORT_CONFIG, NVDM_DATA_ITEM_TYPE_RAW_DATA, (const uint8_t *)&serial_port_setting, sizeof(serial_port_setting_t));

    return SERIAL_PORT_STATUS_OK;
}
#endif

