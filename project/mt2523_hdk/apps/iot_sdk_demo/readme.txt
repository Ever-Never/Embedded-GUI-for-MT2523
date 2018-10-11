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

@par Overview
  - Application description
    - This application demonstrates seven key features of the LinkIt 2523 HDK
      with UI.
  - Application features
    - GNSS. Measures and displays location information continuously.
    - Bluetooth audio. Provides EDR (A2DP, AVRCP, HFP) communication,
      including music control and call control.
    - Bluetooth LE proximity profile (PXP). Demonstrates PXP profile's
      functionality, such as out of range alert.
    - Sensor Subsystem. Demonstrates how to obtain sensor data from physical
      sensors and virtual sensors on a sensor daughterboard, and then display
      the sensor data. MediaTek MT2511 biosensor daughterboard is required to
      demonstrate this feature.
    - Watch face. Demonstrates the low power consumption option of LCM
      partial update.
    - Low power. Demonstrates the low power consumption of Bluetooth, GNSS
      and power saving operations in sleep mode.
    - HID over GATT profile (HOGP). Demonstrates how the HID Device controls
      the behavior of the HID Host.
    - Please refer to the readme file in each sub-folder of the example
      project for a detailed feature description.

@par Hardware and software environment
  - Supported platform
    - LinkIt 2523 HDK
    - LCM module driven by Sitronix ST7789H2 (or another driver IC that
      supports MediaTek 2-data lane interface).
  - HDK switches and pin configuration
    - LCD module's pin mapping is listed below.
      |PIN Name    |GPIOx    |PINx    |
      |------------|---------|--------|
      |  LSRSTB    |  38     |        |
      |  LSCE0_B0  |  39     |        |
      |  LSCK0     |  40     |        |
      |  LSDA0     |  41     |        |
      |  LSA0DA0   |  42     |        |
      |  LPTE      |  43     |        |
  - Environment configuration
    - The output logs are communicated through a type-A to micro-B USB cable
      to the PC from MK20 USB connector on the HDK.
    - Install the mbed serial driver according to the instructions at
      https://developer.mbed.org/handbook/Windows-serial-configuration. For
      more information, please refer to section "Installing the LinkIt 2523
      HDK drivers on Microsoft Windows" on the "LinkIt 2523 HDK User Guide"
      in [sdk_root]/doc folder.
    - Use a type-A to micro-B USB cable to connect type-A USB of the PC and
      MK20 micro-B USB connector on the LinkIt 2523 HDK. For more information
      about the connector cable, please refer to
      https://en.wikipedia.org/wiki/USB#Mini_and_micro_connectors.
    - Launch a terminal emulator program, such as Tera terminal on your PC
      for data logging through UART. For the installation details, please
      refer to section "Installing Tera terminal on Microsoft Windows" on the
      "LinkIt for RTOS Get Started Guide" in [sdk_root]/doc folder.
    - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity:
      none and flow control: off.
    - By default, the application uses UART2 for logging. To change logging
      from UART to USB, enter an AT command "AT+EPORT". By default, the
      application uses USB-COM1 for AT command input. For example, enter
      "AT+EPORT=2,syslog,5", the result "OK" indicates the syslog
      automatically changed at runtime to use USB-COM2. Then enter
      "AT+EPORT=2,syslog,2", the result "OK" indicates the syslog
      automatically changed at runtime to use UART2. USB-COM1 corresponds to
      "MTK USB Modem Port" under Device Manager of the PC. USB-COM2
      corresponds to "MTK USB Debug Port" under Device Manager of the PC.
    - Customization Option
      - \b MTK_DEBUG_LEVEL:  This definition is used to configure system log
                             debug level. The setting will determine whether a
                             debug log will be compiled: empty: All debug logs
                             are compiled. info: All debug logs are compiled.
                             none: All debugs are disabled. By default, All
                             debugs are disabled in the application.
      - \b MTK_PORT_SERVICE_ENABLE:
                             This definition is used to enable the port service
                             feature. By default, the port service feature is
                             enabled in this project. If user wants to disabled
                             port service feature, he/she can set
                             "MTK_PORT_SERVICE_ENABLE = n" in the feature.mk of
                             GCC folder. Port service feature has one
                             dependency that NVDM module must be enabled.
    - Please refer to the readme file in each sub-folder of the example
      project for feature specific requirements (e.g. watch face application
      folder: src/watch_face).
    - Special Easy PinMux Tool (EPT) file configuration
      - A special EPT file is required for the connection to MediaTek MT2511
        biosensor daughterboard (model number: MTK0067). It's located at
        ept_config/LinkIt_2523_iot_sdk_demo.dws.
      - GPIO7 is configured to GPIO function for the boost switch control of
        the daughterboard.
      - GPIO24 is configured to GPIO function with output direction for the
        power down control of the daughterboard.
      - GPIO29 is configured to GPIO function with output direction for the
        reset control of the daughterboard.

@par Directory contents
  - Source and header files
    - \b inc:                  Common header files.
    - \b src/gnss_screen:      GNSS demo source code.
    - \b src/bt_audio:         Bluetooth audio source code.
    - \b src/pxp_screen:       Bluetooth LE PXP source code.
    - \b src/sensor_demo:      Sensor algorithm demo source code.
    - \b src/watch_face:       Watch face demo source code.
    - \b src/low_power_demo:   Low power demo source code.
    - \b src/main.c:           Main program.
    - \b src/sys_init.c:       This file initializes the system, including
                               system clock, UART port for logging, PinMux,
                               cache and other necessary hardware.
    - \b src/ept_gpio_var.c:   The GPIO configuration file generated by the
                               EPT.
    - \b src/ept_eint_var.c:   The EINT configuration file generated by the
                               EPT.
    - \b src/system_mt2523.c:  The configuration file of the Cortex-M4 with
                               floating point unit MCU core registers and system
                               clock.
    - \b src/task_def.c:       Task definition file.
  - Project configuration files using GCC
    - \b GCC/feature.mk:      Feature configuration file.
    - \b Makefile:            Makefile.
    - \b GCC/flash.ld:        Linker script.
    - \b GCC/gnss_screen.mk:  GNSS application makefile.
    - \b GCC/sensor_demo.mk:  Sensor algorithm application makefile.
  - Project configuration files using Keil IDE
    - \b MDK-ARM/iot_sdk_demo.uvoptx:
      uVision5 Project File. Contains the project structure in XML format.
    - \b MDK-ARM/iot_sdk_demo.uvprojx:
      uVision5 project options. Contains the settings for the debugger, trace
      configuration, breakpoints, currently open files, etc.
  - Project configuration files using IAR embedded workbench
    - \b EWARM/iot_sdk_demo.ewd:  Project options.
    - \b EWARM/iot_sdk_demo.ewp:  Project file.
    - \b EWARM/iot_sdk_demo.eww:  Workspace file.
    - \b EWARM/flash.icf:         Linker script.

@par Run the application
  - How to build the iot_sdk_demo application
    - GCC version
      - make command "./build.sh mt2523_hdk iot_sdk_demo" under the SDK root
        folder.
    - Keil IDE
      - Open "iot_sdk_demo.uvprojx" under the MDK-ARM folder and build the
        Keil project.
    - IAR embedded workbench IDE
      - Open "iot_sdk_demo.ewp" under the EWARM folder and build the IAR
        project.
  - How to run each application feature
    - Please refer to the readme file in each sub-folder of the example
      project, such as GNSS demo application folder: src/gnss_screen.
*/
/**
 * @}
 * @}
 * @}
 */
