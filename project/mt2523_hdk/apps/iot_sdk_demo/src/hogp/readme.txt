/* Copyright Statement:
 *
 * (C) 2005-2017 MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its
 * licensors. Without the prior written permission of MediaTek and/or its
 * licensors, any reproduction, modification, use or disclosure of MediaTek
 * Software, and information contained herein, in whole or in part, shall be
 * strictly prohibited. You may only use, reproduce, modify, or distribute
 * (as applicable) MediaTek Software if you have agreed to and been bound by
 * the applicable license agreement with MediaTek ("License Agreement") and
 * been granted explicit permission to do so within the License Agreement
 * ("Permitted User"). If you are not a Permitted User, please cease any
 * access or use of MediaTek Software immediately.
 */

/**
 * @addtogroup mt2523_hdk mt2523_hdk
 * @{
 * @addtogroup mt2523_hdk_apps apps
 * @{
 * @addtogroup mt2523_hdk_apps_iot_sdk_demo iot_sdk_demo
 * @{
 * @addtogroup mt2523_hdk_apps_iot_sdk_demo_ble_hogp ble_hogp
 * @{

@par Overview
  - Application description
    - This is a reference application to demonstrate the HID device operating
      on a HID host through Bluetooth LE. HID device can control apps on
      Android smartphone. For example, to run the Message app and input a
      character "a", simply enter "AT+HOGP=A" AT command.

@par Hardware and software environment
  - Supported platform
    - LinkIt 2523 HDK.
  - Environment configuration
    - The output logs are communicated through a micro-USB cable to the PC
      from MK20 USB connector on the HDK.
    - Install the mbed serial driver according to the instructions at
      https://developer.mbed.org/handbook/Windows-serial-configuration. For
      more information, please refer to section "Installing the LinkIt 2523
      HDK drivers on Microsoft Windows" on the "LinkIt 2523 HDK User Guide"
      in [sdk_root] folder.
    - Use a type-A to micro-B USB cable to connect type-A USB of the PC and
      MK20 micro-B USB connector on the LinkIt 2523 HDK. For more information
      about the connector cable, please refer to
      https://en.wikipedia.org/wiki/USB#Mini_and_micro_connectors.
    - Launch a terminal emulator program, such as Tera terminal on your PC
      for data logging through UART. For the installation details, please
      refer to section "Installing Tera terminal on Microsoft Windows" on the
      "LinkIt for RTOS Get Started Guide" in [sdk_root] folder.
    - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity:
      none and flow control: off.

@par Directory contents
  - Source and header files
    - \b src/hogp/ble_hogp.c:  HOGP profile implementation file.
    - \b inc/hogp/ble_hogp.h:  HOGP header file.

@par Run the example
  - To run the application:
    - 1. Power on the LinkIt 2523 HDK.
    - 2. Launch a terminal emulator program, such as Tera terminal on your PC
      for data logging through UART. The AT command is input by UART.
    - 3. Input AT command "AT+HOGP=START" to start advertising. Search the
      device "HOGP" on the smartphone and tap to connect.
    - 4. Launch the Camera app on the smartphone. Input AT command
      "AT+HOGP=ENTER" to take a picture. Launch the Message app on the
      smartphone, tap on the editor, enter AT command "AT+HOGP=A" to input a
      character "a". The supported AT commands are "AT+HOGP=START",
      "AT+HOGP=STOP", "AT+HOGP=ENTER", "AT+HOGP=LEFT", "AT+HOGP=RIGHT",
      "AT+HOGP=UP", "AT+HOGP=DOWN" and "AT+HOGP=A".
*/
/**
 * @}
 * @}
 * @}
 * @}
 */

