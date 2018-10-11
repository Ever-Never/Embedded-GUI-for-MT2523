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
 * @addtogroup mt2523_hdk_apps_headset_ref_design headset_ref_design
 * @{

@par Overview
  - Application description
    - This application demonstrates headset features based on MediaTek MT2523
      SOC for RTOS development platform. The architecture diagram is shown 
      below.
      @image html headset_ref_design_architecture.png
    - Application features
      - Sink. Enables to connect with a remote device, handle calls and play music
        from the remote device.
      - Battery Service (BAS). Provides the battery status of MT2523 to a remote
        device.
      - Apple Notification Center Service (ANCS). Provides a simple and convenient
        way to access the notifications generated on the iOS devices.
      - Bluetooth Notification. Receives and displays notifications from Android
        devices.
      - Audio Player. Plays local MP3 music.
      - Firmware update Over The Air (FOTA). Uses Bluetooth notification service to 
        receive FOTA packages and trigger an update on the target board.
      - Find Me Profile (FMP). Allows users to find the misplaced device using
        Bluetooth LE.
      - Low Power demo. Provides the method to measure the current consumption
        of different scenario by AT command. User should strictly follow the 
        measuring steps provided in the readme file of low power 
        sub-folder(src\low_power_demo). 
      - The application features are independent, if not otherwise mentioned.
        
@par Hardware and software environment
  - Supported platform
    - LinkIt 2523 HDK.
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
    - By default, the application uses UART0 for logging.
    - By default, the application uses MK20 for AT command input, corresponds 
       to "mbed Serial Port" under Device Manager of the PC.
    - Please contact MediaTek customer support team to find more information
      regarding the pin configuration, hardware peripheral setup, debugger setup and
      more.
  - Customization options and usage
    - No compile option is available to disable an application feature.
    - To disable a feature, delete the feature initialization function in
       the main.c.
        
@par Directory contents
  - Source and header files
    - \b inc:                     Common header files.
    - \b src/ble_bas_app:         Bluetooth LE Battery Service source code.
    - \b src/Notification:        Android notifications source code.
    - \b src/ble_ancs_ios_notification:
                                  ANCS iOS notifications source code.
    - \b src/low_power_demo:      Low Power demo source code.
    - \b src/sink:                Sink source code.
    - \b src/AudioPlayer:         Local audio player source code.
    - \b src/fota:                FOTA source code.
    - \b src/ble_find_me_server:  Find Me server source code.
    - \b src/main.c:              Main program.
    - \b src/ept_gpio_var.c:      The GPIO configuration file generated by the
                                  Easy Pinmux Tool (EPT).
    - \b src/ept_eint_var.c:      The EINT configuration file generated by the
                                  EPT.
    - \b src/system_mt2523.c:     The configuration file of the Cortex-M4 with
                                  floating unit MCU core registers and system
                                  clock.
    - \b src/at_command_serial_port.c:
                                  AT command of port service file.
    - \b src/bt_app_common.c:     Bluetooth application common file.
    - \b src/bt_gattc_discovery.c:
                                  GATT service discovery file.
    - \b src/task_def.c:          Task definition file.
    - \b src/bt_init.c:           Bluetooth initialization file.
    - \b src/gatt_service.c:      GATT and GAP service file.
    - \b src/hci_log.c:           Display HCI log.
  - Project configuration files using GCC
    - \b GCC/feature.mk:          Feature configuration file.
    - \b Makefile:                Makefile.
    - \b GCC/flash.ld:            Linker script.
    - \b GCC/syscalls.c:          MT2523 syscalls for GCC.
    - \b GCC/startup_mt2523.s:    MT2523 startup file for GCC.
  - Project configuration files using Keil IDE
    - \b MDK-ARM/headset_ref_design.uvoptx:
      uVision5 Project File. Contains the project structure in XML format.
    - \b MDK-ARM/headset_ref_design.uvprojx:
      uVision5 project options. Contains the settings for the debugger, trace
      configuration, breakpoints, currently open files, etc.
  - Project configuration files using IAR embedded workbench
    - \b EWARM/headset_ref_design.ewd:  Project options.
    - \b EWARM/headset_ref_design.ewp:  Project file.
    - \b EWARM/headset_ref_design.eww:  Workspace file.
    - \b EWARM/flash.icf:               Linker script.
    
@par Run the application
  - How to build the headset_ref_design application
    - GCC version
      - make command "./build.sh mt2523_hdk headset_ref_design" under
        the SDK root folder.
    - Keil IDE
      - Open "headset_ref_design.uvprojx" under the MDK-ARM folder and build the
        Keil project.
    - IAR embedded workbench IDE
      - Open "headset_ref_design.ewp" under the EWARM folder and build the IAR
        project.    
  - How to download the headset_ref_design application
    - Install and use Flash Tool ("sdk_root/tools/PC_tool_Win.zip")
      to download the application binary headset_ref_design.bin.
  - How to run each feature
    - Power on the MT2523 HDK to initialize and run the features in the
      background.
    - Users can use keypad to trigger sink and audio player actions.
    - BAS, ANCS, Blueooth Notification, FOTA and FMP server can also be 
      triggered on the smartphone. Find more details in each sub-feature
      project's readme file.
*/
/**
 * @}
 * @}
 * @}
 */
