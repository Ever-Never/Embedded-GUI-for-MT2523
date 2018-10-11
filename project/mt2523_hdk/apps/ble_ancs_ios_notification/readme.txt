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
 * @addtogroup mt2523_hdk_apps_ble_ancs_ios_notification ble_ancs_ios_notification
 * @{

@par Overview
  - Application description
    - This application demonstrates a simple and convenient way through
      Bluetooth LE link to access the notifications generated on iOS devices.
      It receives notificaitons, retrieves the notification attributes,
      retrieves the app attributes and performs the notification actions. But
      the notification action is only supported in iOS 8.0 or later.
    - The application requires FreeRTOS.
  - Results
    - The transaction output is in the log.

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
    - \b src/ble_ancs_app.c:      App file, show notifications and attributes.
    - \b src/bt_ancs_common.c:    Implementation of bt stack callback.
    - \b src/ept_eint_var.c:      EINT assignment file with variable names.
    - \b src/ept_gpio_var.c:      GPIO pin assignment with variable names.
    - \b src/bt_hci_log_uart.c:   HCI log.
    - \b src/bt_init.c:           Initialize bt task.
    - \b src/sys_init.c:          Initialize the project configration.
    - \b src/main.c:              Main program.
    - \b src/system_mt2523.c:     MT2523x clock configuration file.
    - \b inc/task_def.h:          Define the task stack size, queue length,
                                  project name, and priority for the
                                  application to create tasks.
    - \b inc/hal_feature_config.h:
                                  MT2523x feature configuration file.
    - \b inc/memory_map.h:        MT2523x memory layout symbol file.
    - \b inc/ept_eint_drv.h:      EINT basic configuration header file.
    - \b inc/ept_gpio_drv.h:      GPIO pin assignment and basic configuration
                                  file.
    - \b inc/ept_keypad_drv.h:    Keypad basic configuration header file.
    - \b inc/main.h:              Main program header file.
    - \b inc/hci_log.h:           HCI log header file.
    - \b GCC/startup_mt2523.s:    MT2523x startup file for GCC.
    - \b GCC/syscalls.c:          MT2523x syscalls for GCC.
    - \b MDK-ARM/startup_mt2523.s:
                                  MT2523x startup file for Keil IDE.
    - \b EWARM/startup_mt2523.s:  MT2523x startup file for IAR.
  - Project configuration files using GCC
    - \b GCC/feature.mk:  Feature configuration.
    - \b GCC/Makefile:    Makefile.
    - \b GCC/flash.ld:    Linker script.
  - Project configuration files using Keil
    - \b MDK-ARM/ble_ancs_ios_notification.uvprojx:
                             uVision5 project file. Contains the project
                             structure in XML format.
    - \b MDK-ARM/flash.sct:  Linker script.
  - Project configuration files using IAR
    - \b EWARM/ble_ancs_ios_notification.ewd:
                           IAR project options. Contains the settings for the
                           debugger.
    - \b EWARM/ble_ancs_ios_notification.ewp:
                           IAR project file. Contains the project structure in
                           XML format.
    - \b EWARM/ble_ancs_ios_notification.eww:
                           IAR workspace file. Contains project information.
    - \b EWARM/flash.icf:  Linker script.

@par Run the example
  - Build the example project with the command "./build.sh mt2523_hdk
    ble_ancs_ios_notification" from the SDK root folder and download the
    binary file to the LinkIt 2523 HDK.
  - Power on LinkIt 2523 HDK.
  - Download and install an app, such as "LightBlue", to connect to iOS
    devices that support Bluetooth 4.0 LE.
  - Search the device "ANCS" in "LightBlue", tap on it to start Bluetooth LE
    connection with the LinkIt 2523 HDK. On the pairing popup on iPhone,
    choose "pair". Notifications will show in the log.
  - The log "*****NOTIFICATION*****Event_id = Added, Event_flags = 0x1d,
    Cat_id = Missed Call, Cat_count = 2, Cat_uuid = 1000" indicates a new
    notification is received.
  - If the iPhone supportes iOS 8.0 or later, input "ancsn" in the COM port
    tool to perform a negative action to the latest notification. The log
    "Perform action success!" indicates the action is performed successfully.
    Similarly, input "ancsp" to perform a possitive action to the latest
    notifiction. Note, that only one action can be performed from the latest
    notification.
*/
/**
 * @}
 * @}
 * @}
 */
