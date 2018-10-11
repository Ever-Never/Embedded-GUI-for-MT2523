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
 * @addtogroup mt2523_hdk_apps_ble_gatt_heart_rate ble_gatt_heart_rate
 * @{

@par Overview
  - Application description
    - This project is a reference application to use the GATTC and GATTS APIs
      to communicate with a Bluetooth LE device.
  - Results
    - The transaction output is collected in a log.

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

@par Directory contents
  - Source and header files
    - \b src/main.c:              Main program.
    - \b src/hrs.c:               HRS source file.
    - \b src/connect_info.c:      Connction info manager for HRS.
    - \b src/hrc.c:               HRC source file.
    - \b src/bt_gattc_connect.c:  Connect manager.
    - \b src/bt_gattc_handle_op.c:
                                  GATTC discovery procedure.
    - \b src/bt_gattc_srv.c:      GATTC service.
    - \b src/hci_log.c:           Show HCI log.
    - \b src/system_mt2523.c:     MT2523x clock configuration file.
    - \b inc/task_def.h:          Define the task stack size, queue length,
                                  project name, and priority for the
                                  application to create tasks.
    - \b inc/hal_feature_config.h:
                                  MT2523x feature configuration file.
    - \b inc/memory_map.h:        MT2523x memory layout symbol file.
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
    - \b MDK-ARM/ble_gatt_heart_rate.uvprojx:
                             uVision5 project file. Contains the project
                             structure in XML format.
    - \b MDK-ARM/ble_gatt_heart_rate.uvoptx:
                             uVision5 project options. Contains the settings
                             for the debugger, trace configuration,
                             breakpoints, currently open files, etc.
    - \b MDK-ARM/flash.sct:  Linker script.
  - Project configuration files using IAR
    - \b EWARM/ble_gatt_heart_rate.ewd:
                           IAR project options. Contains the settings for the
                           debugger.
    - \b EWARM/ble_gatt_heart_rate.ewp:
                           IAR project file. Contains the project structure in
                           XML format
    - \b EWARM/ble_gatt_heart_rate.eww:
                           IAR workspace file. Contains project information.
    - \b EWARM/flash.icf:  Linker script.

@par Run the example
  - Build the application with the command "./build.sh mt2523_hdk
    ble_gatt_heart_rate". from the SDK root folder and download the binary
    file to the LinkIt 2523 HDK.
  - Power on LinkIt 2523 HDK.
  - As a HeartRate Server, Input "a", then search the device "MTKHB" from
    this in Client, then connect. after connected, the heartrate value will
    show in Client.
  - As a HeartRate Client, Input "s", it will start scan and result is show
    in log.
    - The log such as :"heart_rate_scan_cb: -- start: type = 1" ,
      "heart_rate_scan_cb: -- start:addr[0] = 0x5d, addr[1] = 0xfc, addr[2] =
      0x2b, addr[3] = 0x37, addr[4] = 0x9f, addr[5] = 0xee" indicates remote
      device's type and address.
    - Then do connect, input "c type address", the value of type is 0 or 1,
      address is target remote address.
    - After connected, the log "heart_rate_update_notify_cb:start--
      heart_value = ..." will show.
    - If you want to disconnect the link, please input "d".it will disconnect
      this connection.
*/
/**
 * @}
 * @}
 * @}
 */
