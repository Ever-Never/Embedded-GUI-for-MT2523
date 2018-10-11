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
 * @addtogroup mt2523_hdk_apps_bt_spp_transfer_data bt_spp_transfer_data
 * @{
@par Overview
  - Application description
    - This application describes how to use BT SPP API to create a SPP server
      and a SPP client.
    - Results
      - The transaction output is collected in a log and showed in the SP.

@par Hardware and software environment
  - Supported platform
    - LinkIt 2523 HDK.
  - Environment configuration
    - The output logs are communicated through a micro USB cable to the PC
      from MK20 USB connector on the HDK.
    - Install the mbed serial driver according to the instructions at
      https://developer.mbed.org/handbook/Windows-serial-configuration. For
      more information, please refer to section "Installing the LinkIt 2523
      HDK drivers on Microsoft Windows" on the "LinkIt 2523 HDK User Guide"
      in [sdk_root]/doc folder.
      - Use a type-A to micro-B USB cable to connect type-A USB of the PC and
        MK20 micro-B USB connector on the LinkIt 2523 HDK. For more
        information about the connector cable, please refer to
        https://en.wikipedia.org/wiki/USB#Mini_and_micro_connectors.
      - Launch a terminal emulator program, such as Tera terminal on your PC
        for data logging through UART. For the installation details, please
        refer to section "Installing Tera terminal on Microsoft Windows" on
        the "LinkIt for RTOS Get Started Guide" in [sdk_root]/doc folder.
      - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1,
        parity: none and flow control: off.
  - Remote device configuration
    - Install any Bluetooth SPP application that supports both the SPP server
      and the SPP client on the Smartphone from App Store.
    - PC/environment configuration
    - Use SecureCRT or other hyperterminal to output system log.
    - COM port settings. baudrate: 115200, data bits: 8, stop bit: 1, parity:
      none and flow control: off.

@par Directory contents
  - Source and header files
    - \b src/main.c:            Main program.
    - \b inc/main.h:            Main program header file.
    - \b src/system_mt2523.c:   MT2523x's system clock configuration file.
    - \b inc/task_def.h:        Define the task stack size, queue length,
                                project name, and priority for the application
                                to create tasks.
    - \b inc/hal_feature_config.h:
                                MT2523x's feature configuration file.
    - \b src/sys_init.c:        MT2523x's feature configuration file.
    - \b src/ept_gpio_var.c:    GPIO pin assignment with variable names.
    - \b src/ept_eint_var.c:    EINT assignment file with variable names.
    - \b inc/memory_map.h:      MT2523x memory layout symbol file.
    - \b inc/ept_eint_drv.h:    MT2523x EINT basic configuration header file.
    - \b inc/ept_gpio_drv.h:    MT2523x GPIO pin assignment and basic
                                configuration file.
    - \b src/bt_spp_main.c:     BT SPP main file.
    - \b src/bt_spp_client.c:   BT SPP client example file.
    - \b src/bt_spp_server.c:   BT SPP server example file.
    - \b src/bt_init.c:         File to create BT task.
    - \b inc/bt_init.h:         Header file to create BT task.
    - \b src/hci_log.c:         BT SPP show hci log file.
    - \b inc/hci_log.h:         BT SPP show hci log header file.
    - \b inc/bt_spp_main.h:     BT SPP main header file.
    - \b inc/bt_spp_client.h:   BT SPP client example header file.
    - \b inc/bt_spp_server.h:   BT SPP server example header file.
    - \b inc/project_config.h:  BT memory  configuration header file.
    - \b inc/task_def.h:        Tasks definition header file.
  - Project configuration files using GCC
    - \b GCC/feature.mk:        Feature configuration.
    - \b GCC/Makefile:          Makefile.
    - \b GCC/flash.ld:          Linker script.
    - \b GCC/startup_mt2523.s:  MT2523x startup file for GCC.
    - \b GCC/syscalls.c:        MT2523x syscalls for GCC.
  - Project configuration files using Keil
    - \b MDK-ARM/bt_spp_transfer_data.uvprojx:
                             uVision5 project file. Contains the project
                             structure in XML format.
    - \b MDK-ARM/bt_spp_transfer_data.uvoptx:
                             uVision5 project options. Contains the settings
                             for the debugger, trace configuration,
                             breakpoints, currently open files, etc.
    - \b MDK-ARM/flash.sct:  Linker script.
    - \b MDK-ARM/startup_mt2523.s:
                             MT2523x startup file for Keil IDE.
  - Project configuration files using IAR
    - \b EWARM/bt_spp_transfer_data.ewt:
                                  IAR project options. Contains the settings
                                  for the debugger.
    - \b EWARM/bt_spp_transfer_data.ewp:
                                  IAR project file. Contains the project
                                  structure in XML format.
    - \b EWARM/bt_spp_transfer_data.eww:
                                  IAR workspace file. Contains project
                                  information.
    - \b EWARM/flash.icf:         Linker script.
    - \b EWARM/startup_mt2523.s:  MT2523x startup file for IAR.

@par Run the example
  - Build the application with the command "./build.sh mt2523_hdk
    bt_spp_transfer_data".
  - Connect board to PC with serial port cable.
  - Set the correct Smartphone Bluetooth address in the BT SPP client example
    file, then build the example project and download the binary file to the
    LinkIt 2523 development board.
  - Launch SPP application and enable SPP server on the Smartphone.
  - Power on the LinkIt 2523 development board, the SPP client example will
    automatically connect to the Smartphone.
  - The SPP client example will send string of "Hello SPP Server!" to the
    Smartphone.
  - Disconnects the SPP connection from the Smartphone.
  - Initiate the connection to SPP server example from the Smartphone.
  - The SPP server example will send string of "Hello SPP Client!" to the
    Smartphone.
  - Disconnect the SPP connection from the Smartphone.
*/
/**
 * @}
 * @}
 * @}
 */
