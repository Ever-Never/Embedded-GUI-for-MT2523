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

#ifndef ATCI_H
#define ATCI_H
#include "stdint.h"
#include "hal_uart.h"


#ifdef __cplusplus
extern "C"
{
#endif

/**
 *@addtogroup ATCI
 *@{
 * This section introduces the ATCI driver APIs including terms and acronyms, software architecture,
 * supported features, details on how to use this driver, ATCI function groups, enums, structures and functions.
 * The ATCI header file provides the interface for target communication using AT commands though UART.
 * The ATCI module APIs to register the AT command handler, send and receive UART data, and a
 * parser to search for the registered AT command handler. The command handler gets and parses the input AT
 * command data, then performs corresponding operation according to the specified behavior and parameters.
 *
 * @section ATCI_Terms_Chapter ATCI acronyms definition
 *
 * |Acronyms                |Definition                                                   |
 * |-----------------|-------------------------------------|
 * |\b ATCI                  | AT command interface. For more details about the AT command, please refer to <a href="https://en.wikipedia.org/wiki/Hayes_command_set">Hayes command set in Wikipedia.</a>                                |
 * |\b CMD                   | Command.                                                 |
 * |\b URC                   | Unsolicited result code. For the details of URC, please refer to <a href="http://www.itu.int/rec/T-REC-V.25ter-199508-S/en">Data communication over the telephone network.</a>                               |
 * |\b UART                 | Universal Asynchronous Receiver/Transmitter is usually an individual (or part of an) integrated circuit (IC) used for serial communications over a micro-controller unit(MCU) or peripheral device serial port. For an introduction to UART, please refer to <a href="https://en.wikipedia.org/wiki/Uart"> UART in Wikipedia.</a>| 
 *
 *
 * @section ATCI_Features_Chapter Supported features
 * - <b> Supported AT command format </b> \n
 *   The command format should start with "AT+<CMD>", and finish with "<\r><\n>" to mark the end of the
 *   AT command.
 *   The ATCI module supports four modes, as shown below.
 *   - <b>Read command mode </b> \n
 *     Checks the current value or values stored in a parameter. \n
 *     Data format. AT+\<CMD\>?<\\r><\\n>
 *   - <b>Test command mode </b> \n
 *     Tests if a parameter is implemented and checks the supported values. \n
 *     Data format. AT+\<CMD\>=?<\\r><\\n>
 *   - <b>Active command mode </b> \n
 *     Verifies if the action command is implemented. \n
 *     Data format. AT+\<CMD\><\\r><\\n>
 *   - <b>Execute command mode </b> \n
 *     Transfers the parameters in the AT command to a command handler. \n
 *     Data format. AT+\<CMD\>=\<p1\>,\<p2\>...<\\r><\\n>
 *
 *
 * - <b> Supported AT command type </b> \n
 *   - <b> Instruction Type </b> \n
 *     The target receives the AT command and reports the result passively. So the received AT command and a
 *     response must be paired. The target handles the next AT command until the response of current received
 *     AT command is sent.
 *     |Command mode                   |Input command                  |Response command                       |
 *     |-------------------------------|-------------------------------|---------------------------------------|
 *     |<b> Read command mode </b>     |AT+\<CMD\>?<\\r><\\n>            |+\<CMD\>:\<values\><\\r><\\n>OK<\\r><\\n>  |
 *     |<b> Test command mode </b>     |AT+\<CMD\>=?<\\r><\\n>           |+\<CMD\>:\<values\><\\r><\\n>OK<\\r><\\n>  |
 *     |<b> Active command mode </b>   |AT+\<CMD\><\\r><\\n>             |OK<\\r><\\n>                           |
 *     |<b> Execute command mode </b>  |AT+\<CMD\>=\<p1\>,\<p2\>...<\\r><\\n>|OK<\\r><\\n>                           |
 *   \.
 *   - <b> URC Type </b> \n
 *     The target reports the result actively.  It means that target can send any command information through
 *     the ATCI module without receiving any input AT command. The data format is +\<CMD\>:...<\\r><\\n>
 *
 *
 * @section ATCI_archi_Chapter ATCI architecture
 * The software architecture is shown in the figure below.\n
 * @image html atci_architecture.png
 * The ATCI module contains the data transfer and parser sub-modules. The data transfer sub-module
 * can receive UART data, then send to parser sub-module to find the right registered command handler. When the
 * command handler is called to handle input command data, the command handler needs to give a response, then
 * the ATCI data transfer module will send the response data through the UART.
 * If the modules want provide some AT commands, they could use ATCI module register the command handlers to
 * add more AT commands in the target. Also the command handler can send the URC data actively without receiving
 * one input command data.
 *
 *
 * @section ATCI_Folder_Chapter Folder structure
 * The folder structure of ATCI module is shown in the figure below.
 * @image html atci_folder_structure.png
 *   - at_command. Includes source files for the default supported AT command handler provied by IoT platform.
 *   - inc. Inculdes the public header file named atci.h and other ATCI private header files.
 *   - src. Inculdes the source files of the ATCI module.
 *
 *
 * @section ATCI_Usage_Chapter How to use this module
 * - Step 1. Call #atci_init() to initialize the ATCI content and set UART configuration.
 * - Step 2. Call #atci_register_handler() to register the AT command handler.
 * - Step 3. Call xTaskCreate() to create a task that takes atci_def_task() as the entry function where
 *           #atci_processing() executes in a continuous loop.
 * - Step 4. After the above steps, send AT command through UART on the PC. On the target side,
 *           the ATCI module receives UART data and calls the corresponding registered command handler
 *           to provide a response.
 * - Sample code
 * @code
 * // AT CMD table
 * atci_cmd_hdlr_item_t atcmd_table[] = {
 *   {"AT+TEST",   atci_cmd_hdlr_test,  0, 0}
 * };
 *
 * // AT command handler
 *atci_status_t atci_cmd_hdlr_test(atci_parse_cmd_param_t *parse_cmd)
 *{
 *    int read_value = 0;
 *    atci_response_t resonse = {0};
 *    atci_response_t urc_data = {0};
 *    char *param = NULL;
 *    int  param1_val = -1;
 *
 *    resonse.response_flag = 0; // Command execution is complete.
 *
 *    switch (parse_cmd->mode) {
 *        case ATCI_CMD_MODE_TESTING:    // Input data. AT+TEST=?
 *            printf("AT Test OK.\n");
 *            strcpy(resonse.response_buf, "+TEST:(0,1)\r\nOK\r\n");
 *            resonse.response_len = strlen(resonse.response_buf);
 *            atci_send_response(&resonse);
 *            break;
 *
 *        case ATCI_CMD_MODE_READ:    // Input data. AT+TEST?
 *            printf("AT Read done.\n");
 *            sprintf(resonse.response_buf, "+TEST:%d\r\n", read_value);
 *            resonse.response_len = strlen(resonse.response_buf);
 *            // The ATCI appends 'OK' at the end of the response buffer.
 *            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
 *           atci_send_response(&resonse);
 *            break;
 *
 *        case ATCI_CMD_MODE_ACTIVE:  // Input data. AT+TEST
 *            printf("AT Active OK.\n");
 *            // Assume the active mode is invalid and returns an error message.
 *            strcpy(resonse.response_buf, "ERROR\r\n");
 *            resonse.response_len = strlen(resonse.response_buf);
 *            atci_send_response(&resonse);
 *            break;
 *
 *        case ATCI_CMD_MODE_EXECUTION: // Input data. AT+TEST=<p1>  the handler need to parse the parameters.
 *            printf("AT Executing...\r\n");
 *            //Parsing the parameters.
 *            param = strtok(parse_cmd->string_ptr, ",\n\r");
 *            param = strtok(parse_cmd->string_ptr, "AT+TEST=");
 *            param1_val = atoi(param);
 *
 *            if (param != NULL && (param1_val == 0 || param1_val == 1)) {
 *                // Valid parameter, update the data and return "OK".
 *                resonse.response_len = 0;
 *                // // The ATCI appends 'OK' at the end of the response buffer.
 *                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
 *            } else {
 *                // Invalide parameter, return "ERROR".
 *                resonse.response_len = 0;
 *                // // The ATCI appends 'ERROR' at the end of the response buffer.
 *                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
 *            };
 *
 *            atci_send_response(&resonse);
 *            param = NULL;
 *
 *            // Send the URC data.
 *            sprintf(urc_data.response_buf, "+TEST:urc_data\r\n");
 *            urc_data.response_len = strlen(urc_data.response_buf);
 *            urc_data.response_flag |= ATCI_RESPONSE_FLAG_URC_FORMAT;
 *            atci_send_response(&urc_data);
 *
 *            break;
 *
 *        default:
 *            strcpy(resonse.response_buf, "ERROR\r\n");
 *            resonse.response_len = strlen(resonse.response_buf);
 *            atci_send_response(&resonse);
 *            break;
 *    }
 *    return ATCI_STATUS_OK;
 * }
 *
 * // Register the AT CMD handler.
 * void at_cmd_init(void)
 * {
 *   atci_status_t ret = ATCI_STATUS_REGISTRATION_FAILURE;
 *
 *    ret = atci_register_handler(atcmd_table, sizeof(atcmd_table) / sizeof(atci_cmd_hdlr_item_t));
 *    if (ret == ATCI_STATUS_OK) {
 *        printf("at_cmd_init register success\r\n");
 *    } else {
 *        printf("at_cmd_init register fail\r\n");
 *    }
 * }
 *
 * // ATCI task main function.
 * void atci_def_task(void *param)
 * {
 *   while (1) {
 *        atci_processing();
 *    }
 * }
 *
 * void app_main(void)
 * {
 *   // Initialize the ATCI module and set the UART port.
 *   atci_status_t ret = ATCI_STATUS_ERROR;
 *   ret = atci_init(HAL_UART_1);
 *   if (ret == ATCI_STATUS_OK) {
 *   	   // Register the AT CMD handler.
 *       at_cmd_init();
 *   	   // Create a task for the ATCI.
 *   	   xTaskCreate( atci_def_task, "ATCI", 1024, NULL, 3, NULL );
 *   }
 * }
 * @endcode
 *
 */




/** @defgroup atci_define Define
  * @{
  */

/** @brief This macro defines the data length of the AT command response. The length defined in response_buf
  * of #atci_response_t structure cannot be larger than this macro.
  */
#define ATCI_UART_TX_FIFO_BUFFER_SIZE      (1024)


/**
  * @}
  */

/** @defgroup atci_enum Enums
  * @{
  */

/** @brief
 * This enum defines the ATCI status types.
 */
typedef enum {
    ATCI_STATUS_REGISTRATION_FAILURE = -2,   /**< Failed to register the AT command handler table. */
    ATCI_STATUS_ERROR = -1,                  /**< An error occurred during the function call. */
    ATCI_STATUS_OK = 0                       /**< No error occurred during the function call. */
} atci_status_t;

/** @brief
 * This enum defines the input CMD mode.
 */
typedef enum {
    ATCI_CMD_MODE_READ,        /**< Read mode command, such as "AT+CMD?". */
    ATCI_CMD_MODE_ACTIVE,      /**< Active mode command, such as "AT+CMD". */
    ATCI_CMD_MODE_EXECUTION,   /**< Execute mode command, such as "AT+CMD=<op>". */
    ATCI_CMD_MODE_TESTING,     /**< Test mode command, such as "AT+CMD=?". */
    ATCI_CMD_MODE_INVALID      /**< The input command doesn't belong to any of the four types. */
} atci_cmd_mode_t;

/** @brief
 * This enum defines the bit type of the auto append operation used in response_flag of the #atci_response_t structure.
 */
typedef enum {

    ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR = 0x00000002,    /**< Auto append "\r\n" at the end of the response string. */
    ATCI_RESPONSE_FLAG_URC_FORMAT = 0x00000010,           /**< The URC notification flag. */
    ATCI_RESPONSE_FLAG_QUOTED_WITH_LF_CR = 0x00000020,    /**< Auto append "\r\n" at the begining and end of the response string. */
    ATCI_RESPONSE_FLAG_APPEND_OK = 0x00000040,            /**< Auto append "OK\r\n" at the end of the response string. */
    ATCI_RESPONSE_FLAG_APPEND_ERROR = 0x00000080          /**< Auto append "ERROR\r\n" at the end of the response string. */            
} atci_response_flag_t;

/**
  * @}
  */



/** @defgroup atci_struct Structures
  * @{
  */

/** @brief This structure defines the response structure. For more information, please refer to #atci_send_response() function.*/
typedef struct {
    uint8_t  response_buf[ATCI_UART_TX_FIFO_BUFFER_SIZE]; /**< The response data buffer. */
    uint16_t response_len;                                /**< The actual data length of response_buf. */
    uint32_t response_flag;                               /**< For more information, please refer to #atci_response_flag_t. */

} atci_response_t;

/** @brief This structure defines AT command handler input structure. For more information, please refer to #at_cmd_hdlr_fp() function.*/
typedef struct {
    char             *string_ptr;    /**< The input data buffer. */
    uint32_t         string_len;     /**< The response data buffer. */
    uint32_t         name_len;       /**< AT command name length. For example, in "AT+EXAMPLE=1,2,3", name_len = 10 (without symbol "=") */ 
    uint32_t         parse_pos;      /**< The length after detecting the AT command mode. */
    atci_cmd_mode_t mode;            /**< For more information, please refer to #atci_cmd_mode_t. */

} atci_parse_cmd_param_t;

/**
  * @}
  */


/** @defgroup atci_typedef Typedef
  * @{
  */

/** @brief  This defines the callback function prototype. Each AT command should have its own command handler
 *  function. The command handler function is called after the ATCI module receives and parses the correct
 *  AT command.
 *  @param [in] parse_cmd: The value is defined in #atci_parse_cmd_param_t. This parameter is given by the ATCI
 *         parser to indicate the input command data to be transferred to the command handler.
 *  @return    #ATCI_STATUS_OK the command handler successfully processed the input command data. \n
 */
typedef atci_status_t (*at_cmd_hdlr_fp) (atci_parse_cmd_param_t *parse_cmd);
/**
  * @}
  */

/** @addtogroup atci_struct Structures
  * @{
  */
/** @brief This structure defines AT command handler structure. For more information, please refer to #at_cmd_hdlr_fp() function.*/
typedef struct {
    char           *command_head;    /**< AT command string. */
    at_cmd_hdlr_fp command_hdlr;     /**< The command handler, please refer to #at_cmd_hdlr_fp. */
    uint32_t       hash_value1;      /**< Use hash value 1 in the AT command string to accelerate search for the command handler. */
    uint32_t       hash_value2;      /**< Use hash value 2 in the AT command string to accelerate search for the command handler.*/

} atci_cmd_hdlr_item_t;

/** @brief This structure defines AT command handler table structure. For more information, please refer to #atci_register_handler() function.*/
typedef struct {
    atci_cmd_hdlr_item_t *item_table;       /**< For more information, please refer to #atci_cmd_hdlr_item_t. */
    uint32_t              item_table_size;  /**< The command item size in the item table. */

} atci_cmd_hdlr_table_t;

/**
  * @}
  */



/**
 * @brief This function registers the AT command handler. It is used to receive, parse and handle the registered input command data.
 * @param[in] table is the registered handler parameter. For more details about this parameter, please refer to #atci_cmd_hdlr_item_t.
 * @param[in] hdlr_number is the registered table handler size.
 * @return    #ATCI_STATUS_OK the ATCI command handler is successfully registered. \n
 *            #ATCI_STATUS_REGISTRATION_FAILURE duplicate registration of the AT command or an unused registration table will cause failure.
 * @par       Example
 * @code
 *       ret = atci_register_handler(table, hdlr_number);
 *       if (ret == ATCI_STATUS_OK) {
 *          // AT CMD handler is successfully registered.
 *       } else {
 *          // AT CMD handler is failed to register because the duplcated registed AT command or none unused register table.
 *       }
 * @endcode
 */
extern atci_status_t  atci_register_handler(atci_cmd_hdlr_item_t *table, int32_t hdlr_number);

/**
 * @brief This function sends the AT command response data or the URC data.
 * @param[in] response is the response data. For more details about this parameter, please refer to #atci_response_t.
 * @return    #ATCI_STATUS_OK the ATCI module sent data to the UART successfully. \n
 */
extern atci_status_t  atci_send_response(atci_response_t *response);


/**
 * @brief This function initializes the ATCI module. It is used to set the port configuration for data transmission.
 * @param[in] port is the serial port number used to data transmission for ATCI. For more details about this parameter, please refer to #hal_uart_port_t or #serial_port_dev_t.
 * @return   #ATCI_STATUS_OK the ATCI initialized successfully. \n
 *               #ATCI_STATUS_ERROR the UART initialization or the ATCI local initialization failed due to the ATCI initialization failure.
 * @note     The #atci_init() can select the UART through HAL UART port directly, or select UART, USB or BT SPP server/client port through port service for data transmission. 
 * @par       Example
 * @code
 *       ret = atci_init(port);
 *       if (ret == ATCI_STATUS_OK) {
 *          // The ATCI initialized successfully.
 *          atci_register_handler(table, hdlr_number);
 *          // Create an ATCI task.
 *       } else {
 *          // The ATCI initialization failed.
 *       }
 * @endcode
 */
extern atci_status_t     atci_init(uint16_t port);


/**
 * @brief This function parses the input command to find the corresponding command handler and handle the response data.
 * @return    void.
 */
extern void     atci_processing(void);

/**
  * @}
  */
#ifdef __cplusplus
}
#endif


#endif
