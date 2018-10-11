/* Copyright Statement:
 *
 * (C) 2005-2016 MediaTek Inc. All rights reserved.
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
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
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

#ifndef __SERIAL_PORT_H__
#define __SERIAL_PORT_H__

#ifdef MTK_PORT_SERVICE_ENABLE
/**
 * @addtogroup PORT_SERVICE
 * @{
 * This section introduces the Serial Port Service APIs
 * including terms and acronyms, supported features, software
 * architecture, details on how to use this module, enums,
 * structures and functions. The Serial Port Service APIs can be
 * applied to either UART or USB interface. User can easily switch
 * between different devices to match the board configuration
 * through ATCI commands.
 *
 * @section PORT_SERVICE_Terms_Chapter Terms and acronyms
 *
 * |Terms               |Details                                                                 |
 * |--------------------|------------------------------------------------------------------------|
 * |\b ATCI             | AT command interface. For more details about the AT command, please refer to <a href="https://en.wikipedia.org/wiki/Hayes_command_set">Hayes command set in Wikipedia.</a>                                |
 * |\b NVDM             | Non-volatile Data Management is a middleware used to store user data. |
 * |\b UART             | Universal Asynchronous Receiver/Transmitter is usually an individual (or part of an) integrated circuit (IC) used for serial communications over a micro-controller unit (MCU) or peripheral device serial port. For an introduction to UART, please refer to <a href="https://en.wikipedia.org/wiki/Uart"> UART</a> in Wikipedia.|
 * |\b USB              | Universal Serial Bus. USB was designed to standardize the connection of computer peripherals (including keyboards, pointing devices, digital cameras, printers, portable media players, disk drives and network adapters) to personal computers, both to communicate and to supply electric power. It has become commonplace on other devices, such as smartphones, PDAs and video game consoles. USB has effectively replaced a variety of earlier interfaces, such as serial and parallel ports, as well as separate power chargers for portable devices. For more information, please refer to <a href="https://en.wikipedia.org/wiki/USB"> an introduction to the USB in Wikipedia</a>.|
 * |\b OP               | Operation. |
 *
 * @section PORT_SERVICE_Features_Chapter Supported features
 *
 * - <b> Supported Serial Port Service AT command format </b> \n
 *   The command format should start with "AT+EPORT=<OP>", and finish with "<\r><\n>" to
 *   mark the end of the AT command.
 *   The Serial Port Service module AT command's format is "AT+EPORT=<OP>[,<owner_name>,<device_id>,<param1>,<param2>]".
 *   - <b><OP> field description </b> \n
 *     <OP> field has an integer value, each value is described below. \n
 *     - 0: Display the port assignment for application or user, for example, "ATCI = 1" means the ATCI is using port 1. \n
 *     - 1: Set a specific port for a specific application or user. \n
 *     - 2: Switch old port to specific port for the specific application or user at runtime. \n
 *     - 3: Modify the settings of the devices in Serial Port Service. \n
 *     - 4: Show the settings of the devices in Serial Port Service. \n
 *   - <b><owner_name> field description </b> \n
 *     <owner_name> field has a string value for application name of Serial Port Service, details are shown below. \n
 *     - atci: ATCI application. \n
 *     - syslog: Syslog application. \n
 *   - <b><device_id> field description </b> \n
 *     <device_id> field has an integer value. For more details about each value, please refer to #serial_port_dev_t. \n
 *   - <b><param1> field description </b> \n
 *     <param1> field values are described below. \n
 *     - When <OP>=3 and <device_id>=0/1/2/3, it indicates baudrate of the UART port.\n
 *   - <b><param2> field description </b> \n
 *     <param2> field values are described below. \n
 *     - When <OP>=0, it indicates the port assignment of applications or users. \n
 *     - When <OP>=4, it indicates all settings of Serial Port Service devices. \n
 *
 * @section Port_Service_archi_Chapter Port_Service architecture
 *
 * The software architecture is shown in the figure below.\n
 * @image html port_service_architecture.png
 *
 * @section Port_Service_Usage_Chapter How to use this module
 * - Step 1. Call #serial_port_open() to assign the callback function and open the port.
 * - Step 2. Call #serial_port_control() to read data or write data with a specific command.
 * - Step 3. Call #serial_port_close() to close the serial port if the Serial Port Service is no longer in use.
 * - Sample code
 * @code
 *    serial_port_status_t status;
 *    serial_port_read_data_t read_data;
 *    serial_port_write_data_t write_data;
 *    uint32_t g_serial_port_handle = 0;
 *    uint8_t buffer[8] = {0};
 *    uint8_t buffer_len = 8;
 *    serial_port_open_para_t serial_port_open_para;
 *    serial_port_open_para.callback = user_callback;
 *    status = serial_port_open(port, &serial_port_open_para, &g_serial_port_handle);
 *    if (status != SERIAL_PORT_STATUS_OK)
 *        // Error handler
 *    // Read data
 *    read_data.buffer = (uint8_t*) buffer;
 *    read_data.size = buffer_len;
 *    status = serial_port_control(g_serial_port_handle, SERIAL_PORT_CMD_READ_DATA, (serial_port_ctrl_para_t*)&read_data);
 *    if (status != SERIAL_PORT_STATUS_OK)
 *        // Error handler
 *    // Write data
 *    write_data.data = buffer;
 *    write_data.size = buffer_len;
 *    status = serial_port_control(g_serial_port_handle, SERIAL_PORT_CMD_WRITE_DATA, (serial_port_ctrl_para_t*)&write_data);
 *    if (status != SERIAL_PORT_STATUS_OK)
 *        // Error handler
 *
 *    // Close the port if it's no longer in use
 *    status = serial_port_close(g_serial_port_handle);
 *    if (status != SERIAL_PORT_STATUS_OK)
 *        // Error handler
 * @endcode
 * @code
 *     void user_callback (serial_port_dev_t device, serial_port_callback_event_t event, void *parameter)
 *     {
 *        // User code to handle the serial port event.
 *     }
 *  @endcode
 *
 */

#include <stdint.h>
#include "serial_port_config.h"
#include "hal_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup port_service_enum Enums
  * @{
  */

/** @brief
 * This enum defines the port_service device.
 */
typedef enum {
    SERIAL_PORT_DEV_UART_TYPE_BEGIN = 0,                        /**< The first valid UART port in the port service of a device. */
    SERIAL_PORT_DEV_UART_0 = SERIAL_PORT_DEV_UART_TYPE_BEGIN,   /**< UART0 port in the port service of a device. */
    SERIAL_PORT_DEV_UART_1,                                     /**< UART1 port in the port service of a device. */
    SERIAL_PORT_DEV_UART_2,                                     /**< UART2 port in the port service of a device. */
    SERIAL_PORT_DEV_UART_3,                                     /**< UART3 port in the port service of a device. */
    SERIAL_PORT_DEV_UART_TYPE_END = SERIAL_PORT_DEV_UART_3,     /**< The last valid UART port in the port service of a device. */

    SERIAL_PORT_DEV_USB_TYPE_BEGIN,                             /**< The first valid USB port in the port service of a device. */
    SERIAL_PORT_DEV_USB_COM1 = SERIAL_PORT_DEV_USB_TYPE_BEGIN,  /**< USB1 port in the port service of a device. */
    SERIAL_PORT_DEV_USB_COM2,                                   /**< USB2 port in the port service of a device. */
    SERIAL_PORT_DEV_USB_TYPE_END = SERIAL_PORT_DEV_USB_COM2,    /**< The last valid USB port in the port service of a device. */

    SERIAL_PORT_DEV_MAX,                                        /**< The maximum number of ports in the port service of a device. */
    SERIAL_PORT_DEV_UNDEFINED = 255,                            /**< The invalid port number in the port service of a device. */
} serial_port_dev_t;

/** @brief
 * This enum defines the port_service device types.
 */
typedef enum {
    SERIAL_PORT_TYPE_BEGIN = 0,                                 /**< The first port type in the port service. */
    SERIAL_PORT_TYPE_UART = SERIAL_PORT_TYPE_BEGIN,             /**< UART port type in the port service. */
    SERIAL_PORT_TYPE_USB,                                       /**< USB port type in the port service. */
    SERIAL_PORT_TYPE_MAX,                                       /**< The maximum number of port types in the port service. */
} serial_port_type_t;

/** @brief
 * This enum defines the commands for each parameter in #serial_port_ctrl_para_t.
 */
typedef enum {
    SERIAL_PORT_CMD_WRITE_DATA = 0,             /**< Write data command. */
    SERIAL_PORT_CMD_READ_DATA,                  /**< Read data command. */
    SERIAL_PORT_CMD_WRITE_DATA_BLOCKING,        /**< Write data command in blocking mode. */
    SERIAL_PORT_CMD_READ_DATA_BLOCKING,         /**< Read data command in blocking mode. */
    SERIAL_PORT_CMD_GET_WRITE_AVAIL,            /**< UART or USB special command to get the current available space to store the transmit data. */
    SERIAL_PORT_CMD_GET_READ_AVAIL,             /**< UART or USB special command to get the current available size for the received data. */
    SERIAL_PORT_CMD_MAX,                        /**< The maximum number of commands. */
} serial_port_ctrl_cmd_t;

/** @brief
 * This enum defines the events that trigger the user's callback.
 */
typedef enum {
    SERIAL_PORT_EVENT_READY_TO_READ = 0,        /**< Ready to read event. */
    SERIAL_PORT_EVENT_READY_TO_WRITE,           /**< Ready to write event. */
    SERIAL_PORT_EVENT_USB_CONNECTION,           /**< USB connection event. */
    SERIAL_PORT_EVENT_USB_DISCONNECTION,        /**< USB disconnection event. */
} serial_port_callback_event_t;

/** @brief
 * This enum defines the Serial Port Service status.
 */
typedef enum {
    SERIAL_PORT_STATUS_DEV_NOT_READY = -7,      /**< The Serial Port Service device is not ready. */
    SERIAL_PORT_STATUS_BUSY = -6,               /**< The Serial Port Service device is busy. */
    SERIAL_PORT_STATUS_UNINITIALIZED = -5,      /**< The Serial Port Service device is uninitialized. */
    SERIAL_PORT_STATUS_UNSUPPORTED = -4,        /**< The Serial Port Service function is not supported. */
    SERIAL_PORT_STATUS_INVALID_DEVICE = -3,     /**< The Serial Port Service device is invalid. */
    SERIAL_PORT_STATUS_INVALID_PARAMETER = -2,  /**< The Serial Port Service parameters are invalid. */
    SERIAL_PORT_STATUS_FAIL = -1,               /**< The Serial Port Service function has failed. */
    SERIAL_PORT_STATUS_OK = 0,                  /**< The Serial Port Service function executed successfully. */
} serial_port_status_t;
/**
  * @}
  */

/** @defgroup port_service_typedef Typedef
  * @{
  */

/** @brief
 * This defines the callback function prototype for the Serial Port Service. User can not write the code that may cause CPU to block in this callback.
 */
typedef void (*serial_port_register_callback_t)(serial_port_dev_t device, serial_port_callback_event_t event, void *parameter);

/** @brief
 * This defines the serial port handle for the Serial Port Service.
 */
typedef uint32_t serial_port_handle_t;

/**
  * @}
  */


/** @defgroup port_service_struct Structures
  * @{
  */

/** @brief
 * This structure defines the data structure for using #SERIAL_PORT_CMD_WRITE_DATA to
 * write data to the serial port.
 */
typedef struct {
    uint8_t *data;                   /**< The data to write to the serial port. */
    uint32_t size;                   /**< Size of the data to write to the serial port. */
    uint32_t ret_size;               /**< The actual size of data written to the serial port. */
} serial_port_write_data_t;

/** @brief
 * This structure defines the data structure for using #SERIAL_PORT_CMD_READ_DATA to
 * read data from the serial port.
 */
typedef struct {
    uint8_t *buffer;                 /**< The buffer to store the received data. */
    uint32_t size;                   /**< Size of the data to read. */
    uint32_t ret_size;               /**< The actual size of data read from the serial port. */
} serial_port_read_data_t;

/** @brief
 * This structure indicates current available space to put the transmit data in when
 * using #SERIAL_PORT_CMD_UART_GET_WRITE_AVAIL.
 */
typedef struct {
    uint32_t ret_size;               /**< The current available space to put the transmit data in. */
} serial_port_get_write_avail_t;

/** @brief
 * This structure indicates current available data received when
 * using #SERIAL_PORT_CMD_UART_GET_READ_AVAIL.
 */
typedef struct {
    uint32_t ret_size;               /**< The size of the received data. */
} serial_port_get_read_avail_t;

/** @brief
 * This defines the data structure that calls
 * #serial_port_open() to open a Serial Port Service device.
 */
typedef struct {
    serial_port_register_callback_t callback;  /**< The callback function. */
} serial_port_open_para_t;

/** @brief
 * This defines the data structure for communications between the serial port device and its user.
 */
typedef struct {
    char name[SERIAL_PORT_USER_NAME_SIZE];     /**< User name. */
    serial_port_dev_t device;                  /**< Serial Port Service device. */
} serial_port_assign_t;

/** @brief
 * This structure is for configuring baud rate for UART device.
 */
typedef struct {
    hal_uart_baudrate_t baudrate;              /**< UART baud rate. */
} serial_port_setting_uart_t;

/** @brief
 * This structure is for configuring a device in the Serial Port Service.
 */
typedef union {
    serial_port_setting_uart_t port_setting_uart;  /**< UART device. */
} serial_port_dev_setting_t;

/** @brief
 * This structure defines the data structure for each command in
 * #serial_port_ctrl_cmd_t.
 */
typedef union {
    serial_port_write_data_t serial_port_write_data;                /**< Write data parameters. */
    serial_port_read_data_t serial_port_read_data;                  /**< Read data parameters. */
    serial_port_write_data_t serial_port_write_data_blocking;       /**< Write data parameters for blocking mode. */
    serial_port_read_data_t serial_port_read_data_blocking;         /**< Read data parameters for blocking mode. */
    serial_port_get_write_avail_t serial_port_get_write_avail;      /**< Parameters for UART/USB special write command. */
    serial_port_get_read_avail_t serial_port_get_read_avail;        /**< Parameters for UART/USB special read command. */
} serial_port_ctrl_para_t;
/**
  * @}
  */

/**
 * @brief This function initializes the specific device such as UART or USB.
 * @param[in] device is the Serial Port Service device. For more details about this parameter, please refer to #serial_port_dev_t.
 * @param[in] para specifies the callback function. It cannot be NULL.
 * @param[out] handle is the serial port handle for the Serial Port Service.
 * @return    #SERIAL_PORT_STATUS_INVALID_DEVICE, an invalid device error. \n
 *            #SERIAL_PORT_STATUS_INVALID_PARAMETER, an invalid parameter error. \n
 *            #SERIAL_PORT_STATUS_BUSY, the port is busy. \n
 *            #SERIAL_PORT_STATUS_UNSUPPORTED, the port is not supported. \n
 *            #SERIAL_PORT_STATUS_OK, the initialization completed successfully. \n
 * @par       Example
 * @code
 *    serial_port_status_t status;
 *    uint32_t g_serial_port_handle = 0;
 *    serial_port_open_para_t serial_port_open_para;
 *    serial_port_open_para.callback = user_callback;
 *    status = serial_port_open(port, &serial_port_open_para, &g_serial_port_handle);
 *    if (status != SERIAL_PORT_STATUS_OK)
 *        //Error handler
 * @endcode
 * @code
 *    void user_callback (serial_port_dev_t device, serial_port_callback_event_t event, void *parameter)
 *    {
 *       // User code to handle the serial port event
 *    }
 * @endcode
 */
serial_port_status_t serial_port_open(serial_port_dev_t device,
                                      serial_port_open_para_t *para,
                                      serial_port_handle_t *handle);

/**
 * @brief This function controls a specific device to read or write data or
 *        perform other operations supported by the device. This API must be called
 *        after serial_port_open(). User should specify the device with serial port handle.
 *        User must also define its callback to handle events reported by the Serial Port Service.
 * @param[in] handle is the serial port handle for the Serial Port Service.
 * @param[in] command is the command corresponding to a specific parameter. For more details about this parameter, please refer to #serial_port_ctrl_cmd_t.
 * @param[in] para specifies the parameter corresponding to a specific command.
 * @return    #SERIAL_PORT_STATUS_INVALID_PARAMETER, an invalid parameter error. \n
 *            #SERIAL_PORT_STATUS_UNINITIALIZED, the serial port device is uninitialized. \n
 *            #SERIAL_PORT_STATUS_BUSY, the port is busy. \n
 *            #SERIAL_PORT_STATUS_UNSUPPORTED, the port is not supported. \n
 *            #SERIAL_PORT_STATUS_OK, the control operation completed successfully. \n
 * @par       Example
 * @code
 * Sample code, please refer to @ref Port_Service_Usage_Chapter.
 * @endcode
 */
serial_port_status_t serial_port_control(serial_port_handle_t handle,
        serial_port_ctrl_cmd_t command,
        serial_port_ctrl_para_t *para);

/**
 * @brief This function de-initializes a specific device after
 *        using the device to release the resources for other users.
 * @param[in] handle is the serial port handle for the Serial Port Service.
 * @return    #SERIAL_PORT_STATUS_INVALID_PARAMETER, an invalid parameter error. \n
 *            #SERIAL_PORT_STATUS_FAIL, the serial port device close operation failed. \n
 *            #SERIAL_PORT_STATUS_UNSUPPORTED, the port is not supported. \n
 *            #SERIAL_PORT_STATUS_OK, the close operation completed successfully. \n
 * @par       Example
 * @code
 * Sample code please refers to @ref Port_Service_Usage_Chapter.
 * @endcode
 */
serial_port_status_t serial_port_close(serial_port_handle_t handle);

/**
 * @brief This function gets the device type of the serial port device.
 * @param[in] device is the Serial Port Service device.
 * @return    #SERIAL_PORT_TYPE_UART, the Serial Port Service device type is UART. \n
 *            #SERIAL_PORT_TYPE_USB, the Serial Port Service device type is USB. \n
 * @par       Example
 * @code
 *   serial_port_type_t device_type;
 *   device_type = serial_port_get_device_type(device);
 * @endcode
 */
serial_port_type_t serial_port_get_device_type(serial_port_dev_t device);

/**
 * @brief This function dumps device assign list in NVDM, for example, device UART port0 is assigned to user ATCI.
 *        Apply this API to identify the user for each device.
 * @param[in, out] user_count specifies the count of the buffer space pointed to by #port_assign.
 *                 Upon the return of this function, the value in this parameter means the actual
 *                 count of the buffer space being used.
 * @param[out] port_assign the serial port assigned to the user.
 * @return    #SERIAL_PORT_STATUS_FAIL, the function has failed. \n
 *            #SERIAL_PORT_STATUS_OK, the operation completed successfully. \n
 * @par       Example
 * @code
 *   uint32_t user_count;
 *   serial_port_assign_t serial_port_assign[SERIAL_PORT_DEV_MAX];
 *   serial_port_status_t status;
 *   status = serial_port_config_dump_dev_number(&user_count, serial_port_assign);
 *   if (status != SERIAL_PORT_STATUS_OK)
 *      //Error handler
 * @endcode
 */
serial_port_status_t serial_port_config_dump_dev_number(uint32_t *user_count, serial_port_assign_t *port_assign);

/**
 * @brief This function reads assigned device for a specific user.
 * @param[in] user_name is an identifier for the user.
 * @param[out] device is the assigned device for the input user.
 * @return    #SERIAL_PORT_STATUS_INVALID_PARAMETER, an invalid parameter error. \n
 *            #SERIAL_PORT_STATUS_FAIL, the function has failed. \n
 *            #SERIAL_PORT_STATUS_OK, the operation completed successfully. \n
 * @par       Example
 * @code
 *   serial_port_dev_t syslog_port;
 *   serial_port_status_t status;
 *   status = serial_port_config_read_dev_number("syslog", &syslog_port);
 *   if (status != SERIAL_PORT_STATUS_OK)
 *      //Error handler
 * @endcode
 */
serial_port_status_t serial_port_config_read_dev_number(const char *user_name, serial_port_dev_t *device);

/**
 * @brief This function writes new assigned device for a specific user.
 * @param[in] user_name is an identifier for the user.
 * @param[out] device is the assigned device for the input user.
 * @return    #SERIAL_PORT_STATUS_INVALID_PARAMETER, an invalid parameter error. \n
 *            #SERIAL_PORT_STATUS_FAIL, the function has failed. \n
 *            #SERIAL_PORT_STATUS_OK, the operation completed successfully. \n
 * @par       Example
 * @code
 *   serial_port_dev_t syslog_port;
 *   serial_port_status_t status;
 *   syslog_port = SERIAL_PORT_DEV_USB_COM2;
 *   status = serial_port_config_write_dev_number("syslog", syslog_port);
 *   if (status != SERIAL_PORT_STATUS_OK)
 *      //Error handler
 * @endcode
 */
serial_port_status_t serial_port_config_write_dev_number(const char *user_name, serial_port_dev_t device);

/**
 * @brief This function reads current parameters for a specific device.
 * @param[in] device is the device to read its current parameters.
 * @param[out] dev_setting is the current parameters for the input device.
 * @return    #SERIAL_PORT_STATUS_INVALID_PARAMETER, an invalid parameter error. \n
 *            #SERIAL_PORT_STATUS_OK, the operation completed successfully. \n
 * @par       Example
 * @code
 *   serial_port_dev_t device;
 *   serial_port_status_t status;
 *   serial_port_dev_setting_t dev_setting;
 *   status = serial_port_config_read_dev_setting(device, &dev_setting);
 *   if (status != SERIAL_PORT_STATUS_OK)
 *      // Error handler
 * @endcode
 */
serial_port_status_t serial_port_config_read_dev_setting(serial_port_dev_t device, serial_port_dev_setting_t *dev_setting);

/**
 * @brief This function writes new parameters for a specific device.
 * @param[in] device is the device to write its new parameters.
 * @param[in] dev_setting is the new parameters for the input device.
 * @return    #SERIAL_PORT_STATUS_INVALID_PARAMETER, an invalid parameter error. \n
 *            #SERIAL_PORT_STATUS_OK, the operation completed successfully. \n
 * @par       Example
 * @code
 *   serial_port_dev_t device;
 *   serial_port_status_t status;
 *   serial_port_dev_setting_t dev_setting;
 *   status = serial_port_config_read_dev_setting(device, &dev_setting);
 *   if (status != SERIAL_PORT_STATUS_OK)
 *      //Error handler
 *   status = serial_port_config_write_dev_setting(device, &dev_setting);
 *   if (status != SERIAL_PORT_STATUS_OK)
 *      //Error handler
 * @endcode
 */
serial_port_status_t serial_port_config_write_dev_setting(serial_port_dev_t device, serial_port_dev_setting_t *dev_setting);

/**
  * @}
  */

#endif

#ifdef __cplusplus
}
#endif

#endif

