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

#ifndef GNSS_API_H
#define GNSS_API_H
#include <stdint.h>

/**
 *@addtogroup GNSS
 *@{
 * This section introduces the GNSS interface APIs including terms and acronyms, supported features, software architecture, details on how to use the GNSS interface, GNSS function groups, enums, structures and functions.
 * The GNSS API provides a unified method to control and communicate the data with GNSS chip. 
 * For more details on the data format, data flow and other information on GNSS, please refer to MediaTek LinkIt™ Development Platform for RTOS GNSS Developer's Guide.
 *
 * Terms and Acronyms
 * ======
 * |Terms                   |Details                                                                 |
 * |------------------------|------------------------------------------------------------------------|
 * |\b GPS                  | Global Positioning System (GPS) satellites broadcast microwave signals to enable GPS receivers on or near the Earth's surface to determine location, velocity and time. For an introduction to GPS, please refer to <a href="https://en.wikipedia.org/wiki/GPS_signals"> GPS in Wikipedia.</a>.|
 * |\b UTC                  | Coordinated Universal Time is the primary time standard to adjust world clocks and time. For an introduction to UTC, please refer to <a href="https://en.wikipedia.org/wiki/UTC_offset"> UTC in Wikipedia.</a>.|
 * |\b NMEA                 | National Marine Electronics Association (NMEA 0183) is a combined electrical and data specification for communication between marine electronic devices such as echo sounder, sonar, anemometer, gyrocompass, autopilot, GNSS receivers and many other types of instruments. For an introduction to NMEA, please refer to <a href="https://en.wikipedia.org/wiki/NMEA_0183"> NMEA in Wikipedia.</a>.|
 * |\b UART                 | Universal Asynchronous Receiver/Transmitter is usually an individual (or part of an) integrated circuit (IC) used for serial communications over a micro-controller unit(MCU) or peripheral device serial port. For an introduction to UART, please refer to <a href="https://en.wikipedia.org/wiki/Uart"> UART in Wikipedia.</a>.|
 * |\b PMTK                 | PMTK command is MediaTek proprietary data transfer protocol for GNSS. It enables configuring the parameters of the GNSS chipset, aiding assistance position information and receive notifications from the GNSS chip.</a>.|
 * |\b EINT                 | External Interrupt Controller. Process the interrupt request from external source or peripheral.|
 * |\b GNSS                 | Global navigation satellite system is a satellite navigation system with global coverage, such as GPS.|
 *
 * @section GNSS_Architechture_Chapter Software architecture of GNSS.
 * The GNSS chip is a System-in-Package (SiP) in LinkIt 2523 board by SAC that connects with the host using UART.
 *
 * @section GNSS_Usage_Chapter How to use GNSS interface.
 * - step 1: Call #gnss_init() to initialize the GNSS content and register a callback function.
 * - step 2: Call #gnss_power_on() to power on the GNSS chip.
 * - step 3: Call #gnss_send_command() to configure GNSS chip settings, such as aiding time.
 * - step 4: Call #gnss_read_sentence() to read the incoming NMEA data.
 * - step 5: Call #gnss_power_off() to power off the GNSS chip.
 * - Sample code:
 * @code
 * int32_t rx_data_ready = 0;
 * int32_t tx_data_ready = 1;
 * void my_callback_func(gnss_callback_type_t type, void *param)
 * {
 *     if (type == GNSS_NOTIFICATION_TYPE_READ) {
 *         // The incoming NMEA sentence is available to read.
 *         // Set a flag, such as send a message to the task and then call #gnss_read_sentence() to read the NMEA sentence.
 *         rx_data_ready = 1;
 *     }
 *     if (type == GNSS_NOTIFICATION_TYPE_WRITE) {
 *         // Set a flag, such as send a message to the task and then call #gnss_send_command() to send a command to GNSS chip.
 *         tx_data_ready = 1;
 *     }
 *     if (type == GNSS_NOTIFICATION_TYPE_POWER_ON_CNF) {
 *         if (*((int32_t*) param) == 1){
 *             // Power on successfully completed.
 *         }
 *     }
 *     if (type == GNSS_NOTIFICATION_TYPE_POWER_OFF_CNF) {
 *         if (*((int32_t*) param) == 1){
 *             // Power off successfully completed.
 *         }
 *     }
 * }
 *
 * void app_main(void)
 * {
 *    int32_t ret;
 *    int8_t buf[256];
 *    int32_t send_len = 0;
 *    uint8_t command[] = "$PMTK353,1,0,0,0,0*2A\r\n";
 *    // Initilize the GNSS content and register a callback function.
 *    gnss_init(my_callback_func);
 *    //Power on the GNSS chip.
 *    gnss_power_on();
 *    do {
 *        // Waiting for the incoming NMEA data.
 *        while (!rx_data_ready);
 *        // Read a complete NMEA sentence, if there is no data, the return value is 0.
 *        ret = gnss_read_sentence(buf, 256);
 *        if (ret > 0) {
 *        // Process the NMEA sentence.
 *        } else {
 *            // Wait
 *        }
 *
 *        do {
 *            // Send
 *            send_len += gnss_send_command(command + send_len, strlen(command) - send_len);
 *            if (send_len == strlen(command)) {
 *                // Data is successfully sent.
 *                break;
 *            } else {
 *                // Set the TX flag.
 *                tx_data_ready = 0;
 *                // Waiting for data send to complete.
 *                while(!tx_data_ready);
 *            }
 *        while (1);
 *    } while (1);
 *    // Power off the GNSS chip.
 *    gnss_power_off();
 * }
 * @endcode
 *
 */

/** @brief This enum notifies the user about an incoming event from the GNSS chip.
 * @sa gnss_notification_type_t gnss_init()
 *
 */
typedef enum {
    GNSS_NOTIFICATION_TYPE_POWER_ON_CNF,  /**< The event notification is sent once the chip is powered on.*/
    GNSS_NOTIFICATION_TYPE_POWER_OFF_CNF, /**< The event notification is sent once the chip is powered off.*/
    GNSS_NOTIFICATION_TYPE_READ,      /**< The event notification is sent if incoming GNSS data is available.*/
    GNSS_NOTIFICATION_TYPE_WRITE,     /**< Notifies the host to send a command again.*/
    GNSS_NOTIFICATION_TYPE_HOST_WAKEUP,   /**< The GNSS chip is ready to send positioning data to the host. The host should stay on to receive the data.*/
    GNSS_NOTIFICATION_TYPE_DEBUG_INFO,    /**< Notify the user to save debugging data.*/
} gnss_notification_type_t;

/** @brief Data structure to carry the debugging data.
 * @sa gnss_notification_type_t gnss_notification_type_t
 *
 */
typedef struct {
    int32_t length;   /**< Debugging data length.*/
    int8_t* debug_data;  /**< Pointer to the debug data.*/
} gnss_debug_data_t;

/** @brief GNSS callback typedef, the registered callback is invoked when a UART or EINT interrupt is triggered.
 * @sa gnss_init
 *
 */
typedef void (*gnss_callback_t)(gnss_notification_type_t type, void *param);

/**
 * @brief 
 *  This function initializes the GNSS content. This function does not support multi-instance. 
 *  The subsequent call to this function will overwrite the callback function registered by the previous call to this function.
 * @param[in] callback_function the callback function to be registered. The callback is invoked when a UART or EINT interrupt occurs.
 * @sa gnss_notification_type_t
 */
extern void gnss_init(gnss_callback_t callback_function);

/**
 * @brief
 * This function powers on the GNSS chip.
 * @param void
 * @return
 */
extern void gnss_power_on(void);

/**
 * @brief
 * This function powers off the GNSS chip.
 * @param void
 * @return
 */
extern void gnss_power_off(void);

/**
 * @brief
 * This function sends a PMTK command to the GNSS chip.
 * "$PMTK225,0,0,0*2B\r\n"
 * - \b Example \b Commands.\n\n
 * Packet Type: 225 PMTK_SET_PERIODIC_MODE periodic power saving mode settings.\n
 * Packet Type: 740 PMTK_DT_UTC Time aiding. The packet contains the current UTC time.\n
  *\n
 * For the details on the command, please refer to MediaTek LinkIt™ Development Platform for RTOS GNSS Developer's Guide.
 * This is an asynchronous API.
 *
 * @param[in] cmd The pointer to a buffer of the PMTK command.
 * @param[in] cmd_len lengh of the PMTK command.
 * @return length of the data sent.
 */
extern int32_t gnss_send_command(int8_t *cmd, int32_t cmd_len);

/**
 * @brief This function reads the incoming NMEA sentence.
 *        It returns a complete NMEA sentence.
 *        The NMEA sentence can be processed. The sentence format is aligned with the NMEA0183 protocol.
 *        The NMEA sentence: "$GPGGA,030544.713,,,,,0,0,,,M,,M,,*4B", For more details about NMEA, please refer to MediaTek LinkIt™ Development Platform for RTOS GNSS Developer's Guide.
 * @param[in,out] data_buffer a user defined buffer allocated for the NMEA sentence.
 * @param[in] length is the buffer size.
 *            If the buffer size is smaller than the NMEA sentence length, the NMEA sentence will be truncated, up to the size.
 *            Usually the size is smaller than 256 bytes. Users can allocate a buffer the size bigger than 256 bytes.
 * @return the read sentence's length.
 */
extern int32_t gnss_read_sentence (int8_t *data_buffer, int32_t length);

/**
 * @}
 */
#endif /*GNSS_API_H*/

