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

/* Includes ------------------------------------------------------------------*/
#include "at_command.h"
#include "serial_port.h"
#include "toi.h"
#include "syslog.h"
#include "string.h"

/* Private macro -------------------------------------------------------------*/
#define AT_COMMAND_SERIAL_PORT_HELP \
"AT+EPORT - show or modify serial port assignment and switch.\r\n\
usage:\r\n\
   Serial port show:   AT+EPORT=0\r\n\
   Serial port assign: AT+EPORT=1,<owner_name>,<device_id>\r\n\
   Serial port switch: AT+EPORT=2,<owner_name>,<device_id>\r\n\
   Parameters modify:  AT+EPORT=3,<device_id>,<parameters>\r\n\
   Parameters show:    AT+EPORT=4\r\n"

log_create_module(atci_serialport, PRINT_LEVEL_INFO);

#define LOGE(fmt,arg...)   LOG_E(atci_serialport, "[serialport]"fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atci_serialport, "[serialport]"fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atci_serialport ,"[serialport]"fmt,##arg)

#define is_0_to_f(_c)   ((_c >= '0' && _c <= '9') || (_c >= 'a' && _c <= 'f'))
#define ascii_to_number(_c)   ((_c >= '0' && _c <= '9') ? (_c - '0') : (_c - 'a' + 10))
#define number_to_ascii(_c)   ((_c <= 9) ? (_c + '0') : (_c - 10 + 'a'))

/* Private typedef -----------------------------------------------------------*/
typedef struct {
    char name[SERIAL_PORT_USER_NAME_SIZE];
    void (*serial_port_switch)(serial_port_dev_t device);
} serial_port_switch_t;

typedef struct {
    bool (*parse_para)(serial_port_dev_t device, char *parameter, serial_port_dev_setting_t *dev_setting);
    void (*show_para)(serial_port_dev_t device, serial_port_dev_setting_t *dev_setting, char *buffer);
} serial_port_atcmd_operation_t;

/* Private functions ---------------------------------------------------------*/
static bool serial_port_uart_parse_para(serial_port_dev_t device, char *parameter, serial_port_dev_setting_t *dev_setting);
static bool serial_port_usb_parse_para(serial_port_dev_t device, char *parameter, serial_port_dev_setting_t *dev_setting);
static void serial_port_uart_show_para(serial_port_dev_t device, serial_port_dev_setting_t *dev_setting, char *buffer);
static void serial_port_usb_show_para(serial_port_dev_t device, serial_port_dev_setting_t *dev_setting, char *buffer);

/* Extern functions ---------------------------------------------------------*/
extern void syslog_serial_port_switch(serial_port_dev_t device);
extern void atci_serial_port_switch(serial_port_dev_t device);

/* Private variables ---------------------------------------------------------*/
static serial_port_switch_t g_serial_port_switch[] = {
    {
        "syslog",
        syslog_serial_port_switch,
    },
    {
        "atci",
        atci_serial_port_switch,
    },
};

static serial_port_atcmd_operation_t g_serial_port_atcmd_operation[SERIAL_PORT_TYPE_MAX] = {
    {
        serial_port_uart_parse_para,
        serial_port_uart_show_para,
    },
    {
        serial_port_usb_parse_para,
        serial_port_usb_show_para,
    },
};

/**
* @brief      Reassign the specific user of port service to use new device port.
* @param[in]  owner_name: The specific user of port service.
* @param[in]  dev_id: The new device port to use.
* @return     whether Reassign of device port is successfully.
*/
static bool at_serial_port_switch(char *owner_name, serial_port_dev_t dev_id)
{
    uint32_t i;

    for (i = 0; i < sizeof(g_serial_port_switch) / sizeof(serial_port_switch_t); i++) {
        if (strcmp(g_serial_port_switch[i].name, owner_name) == 0) {
            g_serial_port_switch[i].serial_port_switch(dev_id);
            return true;
        }
    }

    return false;
}

/**
* @brief      Translate integer to string.
* @param[in]  number: The integer need to tranlate.
* @param[out] buffer: The buffer used to store the string translated from integer.
* @return     bit count of the string.
*/
static uint32_t digital_to_string(uint32_t number, char *buffer)
{
    uint32_t i, bits, temp[10], divider;

    bits = 0;
    divider = number;
    do {
        temp[bits] = divider % 10;
        divider /= 10;
        bits++;
    } while (divider);

    for (i = 0; i < bits; i++) {
        buffer[i] = temp[bits - i - 1] + 0x30;
    }

    return bits;
}

/**
* @brief      Parse UART specifc parameters string and store result to structure of port service.
*             Pattern of Parameters string for UART shows below:
*             <baudrate>
* @param[in]  device: the UART specific device index.
* @param[in]  parameter: parameters string for the specific device.
* @param[out] dev_setting: parameter setting structure of the specific device to store result of parse.
* @return     whether parse of parameter string is successfully.
*/
static bool serial_port_uart_parse_para(serial_port_dev_t device, char *parameter, serial_port_dev_setting_t *dev_setting)
{
    char *p_curr_pos;
    uint8_t type;
    uint32_t temp;

    p_curr_pos = parameter;
    while (*p_curr_pos && (*p_curr_pos != '\n') && (*p_curr_pos != '\r')) {
        p_curr_pos++;
    }
    *p_curr_pos = '\0';

    temp = toi(parameter, &type);
    if (!(type == TOI_DEC && temp < HAL_UART_BAUDRATE_MAX)) {
        return false;
    }

    dev_setting->port_setting_uart.baudrate = (hal_uart_baudrate_t)temp;

    return true;
}

/**
* @brief      Parse USB specifc parameters string and store result to structure of port service.
*             Now it's just reserved.
* @param[in]  device: the USB specific device index.
* @param[in]  parameter: parameters string for the specific device.
* @param[out] dev_setting: parameter setting structure of the specific device to store result of parse.
* @return     whether parse of parameter string is successfully.
*/
static bool serial_port_usb_parse_para(serial_port_dev_t device, char *parameter, serial_port_dev_setting_t *dev_setting)
{
    return false;
}

/**
* @brief      Format parameter setting of the specific UART device and store in specific buffer.
* @param[in]  device: the specific device index.
* @param[in]  dev_setting: parameter setting structure of the specific device.
* @param[out] buffer: the buffer used to store formatted data.
* @return     None.
*/
static void serial_port_uart_show_para(serial_port_dev_t device, serial_port_dev_setting_t *dev_setting, char *buffer)
{
    uint32_t bits;
    char temp[10];

    strcat(buffer, "baudrate = ");
    bits = digital_to_string(dev_setting->port_setting_uart.baudrate, temp);
    temp[bits] = '\0';
    strcat(buffer, temp);
}

/**
* @brief      Format parameter setting of the specific USB device and store in specific buffer.
* @param[in]  device: the specific device index.
* @param[in]  dev_setting: parameter setting structure of the specific device.
* @param[out] buffer: the buffer used to store formatted data.
* @return     None.
*/
static void serial_port_usb_show_para(serial_port_dev_t device, serial_port_dev_setting_t *dev_setting, char *buffer)
{
    strcat(buffer, "none");
}

/**
* @brief      Parse device common parameters string imported by ATCI input.
* @param[in]  para: parameters string imported by ATCI input.
* @param[out] owner_name: ower name of port service user after parsing.
* @param[out] dev_id: device index assigned to port service user after parsing.
* @return     whether parse of parameter string is successfully.
*/
static bool parse_command(char *para, char *owner_name, serial_port_dev_t *dev_id)
{
    uint8_t type;
    uint32_t temp;
    char *p_owner_name, *p_device_id, *p_bt_address, *p_curr_pos;

    /* skip AT+EPORT=2 */
    p_owner_name = p_curr_pos = para;
    p_curr_pos = strchr(p_owner_name, ',');
    if (p_curr_pos == NULL) {
        return false;
    }

    /* skip <owner_name> */
    p_owner_name = ++p_curr_pos;
    p_curr_pos = strchr(p_owner_name, ',');
    if (p_curr_pos == NULL) {
        return false;
    }
    *p_curr_pos = '\0';

    /* skip <device_id> */
    p_device_id = ++p_curr_pos;
    p_curr_pos = strchr(p_device_id, ',');
    if (p_curr_pos != NULL) {
        *p_curr_pos = '\0';
        p_bt_address = ++p_curr_pos;
        p_curr_pos = p_bt_address;
    } else {
        p_bt_address = NULL;
        p_curr_pos = p_device_id;
    }

    /* Replace end of parameter string to '\0' */
    while (*p_curr_pos && (*p_curr_pos != '\n') && (*p_curr_pos != '\r')) {
        p_curr_pos++;
    }
    *p_curr_pos = '\0';

    /* extract owner name from parameters string */
    if (strlen(p_owner_name) >= SERIAL_PORT_USER_NAME_SIZE) {
        return false;
    }
    strcpy(owner_name, p_owner_name);

    /* extract device index from parameters string */
    temp = toi(p_device_id, &type);
    if (!(type == TOI_DEC && temp <= SERIAL_PORT_DEV_USB_TYPE_END)) {
        return false;
    }
    *dev_id = (serial_port_dev_t)temp;

    return true;
}

/**
* @brief      Parse and store parameters string imported by ATCI input.
* @param[in]  para: parameters string imported by ATCI input.
* @return     whether parse of parameter string is successfully.
*/
static bool parse_parameter_modify_command(char *para)
{
    uint8_t type;
    uint32_t temp;
    char *p_owner_name, *p_device_id, *p_parameter, *p_curr_pos;
    serial_port_dev_t device;
    serial_port_type_t device_type;
    serial_port_dev_setting_t dev_setting;

    /* skip AT+EPORT=3. */
    p_owner_name = p_curr_pos = para;
    p_curr_pos = strchr(p_owner_name, ',');
    if (p_curr_pos == NULL) {
        return false;
    }

    /* skip <device_id>. */
    p_device_id = ++p_curr_pos;
    p_curr_pos = strchr(p_device_id, ',');
    if (p_curr_pos == NULL) {
        return false;
    }
    *p_curr_pos = '\0';

    /* skip <parameters>. */
    p_parameter = ++p_curr_pos;

    /* Replace end of parameter string to '\0' */
    while (*p_curr_pos && (*p_curr_pos != '\n') && (*p_curr_pos != '\r')) {
        p_curr_pos++;
    }
    *p_curr_pos = '\0';

    temp = toi(p_device_id, &type);
    if (!(type == TOI_DEC && temp <= SERIAL_PORT_DEV_USB_TYPE_END)) {
        return false;
    }
    device = (serial_port_dev_t)temp;

    /* Call device specific parse function to parse those parameter string. */
    serial_port_config_read_dev_setting(device, &dev_setting);
    device_type = serial_port_get_device_type(device);
    if (g_serial_port_atcmd_operation[device_type].parse_para(device, p_parameter, &dev_setting) == false) {
        return false;
    }
    serial_port_config_write_dev_setting(device, &dev_setting);

    return true;
}

/**
* @brief      Read and format default port setting of all port service devices.
* @param[out] response_buf: Buffer to store all formatted data.
* @return     None.
*/
static void at_serial_port_show_setting(char *response_buf)
{
    uint32_t i, user_count, bits;
    char temp[10];
    serial_port_assign_t serial_port_assign[SERIAL_PORT_DEV_MAX];
    serial_port_status_t status;

    status = serial_port_config_dump_dev_number(&user_count, serial_port_assign);

    if (status != SERIAL_PORT_STATUS_OK) {
        strcat(response_buf, " none\r\n ");
        return;
    }

    for (i = 0; i < user_count; i++) {
        strcat(response_buf, "+EPORT: ");
        strcat(response_buf, serial_port_assign[i].name);
        strcat(response_buf, " = ");
        bits = digital_to_string(serial_port_assign[i].device, temp);
        temp[bits] = '\0';
        strcat(response_buf, temp);
        strcat(response_buf, "\r\n");
    }
}

/**
* @brief      Read and format all parameters setting of port service devices.
* @param[out] response_buf: Buffer to store all formatted data.
* @return     None.
*/
static void at_serial_port_show_para_setting(char *response_buf)
{
    serial_port_dev_t device;
    serial_port_type_t device_type;
    uint32_t bits;
    char temp[128];
    serial_port_dev_setting_t dev_setting;

    for (device = SERIAL_PORT_DEV_UART_TYPE_BEGIN; device <= SERIAL_PORT_DEV_USB_TYPE_END; device++) {
        /* Read all parameters setting of port service devices. */
        serial_port_config_read_dev_setting(device, &dev_setting);
        /* Format those setting and store to buffer imported. */
        strcat(response_buf, "+EPORT: ");
        bits = digital_to_string(device, temp);
        temp[bits] = '\0';
        strcat(response_buf, temp);
        strcat(response_buf, "\r\n");
        memset(temp, 0, sizeof(temp));
        device_type = serial_port_get_device_type(device);
        g_serial_port_atcmd_operation[device_type].show_para(device, &dev_setting, temp);
        strcat(response_buf, temp);
        strcat(response_buf, "\r\n");
    }
}

/**
* @brief      AT command handler function for port service.
* @param[in]  parse_cmd: command parameters imported by ATCI module.
* @return     Execution result of port service AT command.
*/
atci_status_t atci_cmd_hdlr_serial_port(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse = {{0}};
    char owner_name[SERIAL_PORT_USER_NAME_SIZE];
    serial_port_dev_t dev_id;
    bool result;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:
            LOGI("atci_cmd_hdlr_serial_port query command usage\r\n");
            strcpy((char *)(resonse.response_buf), AT_COMMAND_SERIAL_PORT_HELP);
            resonse.response_len = strlen((char *)(resonse.response_buf));
            break;
        case ATCI_CMD_MODE_EXECUTION:
            /* Show default port for specific port service user. */
            if (strncmp((char *)parse_cmd->string_ptr, "AT+EPORT=0", 10) == 0) {
                LOGI("atci_cmd_hdlr_serial_port read port config\r\n");
                at_serial_port_show_setting((char *)(resonse.response_buf));
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            }
            /* Modify default port for specific port service user. */
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EPORT=1,") != NULL) {
                LOGI("atci_cmd_hdlr_serial_port write port config\r\n");
                result = parse_command(parse_cmd->string_ptr, owner_name, &dev_id);
                if (result == false) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                serial_port_config_write_dev_number(owner_name, dev_id);
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            }
            /* Switch port for specific port service user. */
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EPORT=2,") != NULL) {
                LOGI("atci_cmd_hdlr_serial_port switch port\r\n");
                result = parse_command(parse_cmd->string_ptr, owner_name, &dev_id);
                if (result == false) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(&resonse);
                at_serial_port_switch(owner_name, dev_id);
                return ATCI_STATUS_OK;
            }
            /* Modify parameters setting of specific port service device. */
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EPORT=3,") != NULL) {
                LOGI("atci_cmd_hdlr_serial_port modify parameters\r\n");
                result = parse_parameter_modify_command(parse_cmd->string_ptr);
                if (result == false) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            }
            /* Dump parameters setting of all port service devices. */
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EPORT=4") != NULL) {
                LOGI("atci_cmd_hdlr_serial_port show parameters\r\n");
                at_serial_port_show_para_setting((char *)(resonse.response_buf));
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else {
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            break;
        default:
            strcpy((char *)(resonse.response_buf), "ERROR\r\n");
            resonse.response_len = strlen((char *)(resonse.response_buf));
            break;
    }

    atci_send_response(&resonse);

    return ATCI_STATUS_OK;
}

#endif

