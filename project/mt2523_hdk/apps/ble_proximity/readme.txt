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
 * @addtogroup mt2523_hdk_apps_ble_proximity ble_proximity
 * @{

@par Overview
  - Application description
    - This example project demonstrates the basic functions of Proximity
      profile (PXP).

@par Hardware and software environment
  - Supported platform
    - LinkIt 2523 HDK.
  - Environment configuration
    - Need a serial tool (such as Tera terminal) for UART logging.
    - COM port settings: baudrate: 115200, data bits: 8, stop bit: 1, parity:
      none and flow control: off.
  - Peer test device
    - Download and install an app, such as "LightBlue", to connect to devices
      that support Bluetooth 4.0 Low Energy, on iPhone 6S Plus or later
      version device.

@par Directory contents
  - Source and header files
    - \b src/main.c:              Main program.
    - \b src/ble_gatt_common.c:   GATT server common function handle file.
    - \b src/ble_gatt_connect.c:  Bluetooth LE connection file.
    - \b src/ble_gatt_service.c:  GATT and GAP service file.
    - \b src/ble_gatt_srv.c:      GATT extern API file.
    - \b src/ble_gatt_utils.c:    GATT utilities file.
    - \b src/ble_lls.c:           Link loss service file.
    - \b src/bt_init.c:           Bluetooth initialization file.
    - \b src/ble_pxp_main.c:      Main program for PXP.
    - \b src/ept_eint_var.c:      EINT assignment file with variable names.
    - \b src/ept_gpio_var.c:      GPIO pin assignment with variable names.
    - \b src/system_mt2523.c:     MT2523x system clock configuration file.
    - \b GCC/startup_mt2523.s:    MT2523x startup file for GCC.
    - \b GCC/syscalls.c:          MT2523x syscalls for GCC.
    - \b inc/task_def.h:          Define the task stack size, queue length,
                                  project name, and priority for the
                                  application to create tasks.
    - \b inc/hal_feature_config.h:
                                  MT2523x feature configuration file.
    - \b inc/memory_map.h:        MT2523x memory layout symbol file.
    - \b inc/ept_eint_drv.h:      EINT configuration header file.
    - \b inc/ept_gpio_drv.h:      GPIO pin assignment and configuration file.
    - \b inc/ept_keypad_drv.h:    Keypad configuration header file.
    - \b inc/main.h:              Main program header file.
    - \b inc/hci_log.h:           HCI log header file.
    - \b inc/ble_gatt_common.h:   GATT server common function handle file
                                  header file.
    - \b inc/ble_gatt_connect.h:  Bluetooth LE connection header file.
    - \b inc/ble_gatt_service.h:  GATT and GAP service header file.
    - \b inc/ble_gatt_srv.h:      GATT extern API header file.
    - \b inc/ble_gatt_srv_internal.h:
                                  GATT service header file.
    - \b inc/ble_gatt_utils.h:    GATT utilities header file.
    - \b inc/ble_pxp_main.h:      Main program header file for PXP.
    - \b inc/ble_lls.h:           GATT service LLS header file.
  - Project configuration files using GCC.
    - \b GCC/feature.mk:  Feature configuration.
    - \b GCC/Makefile:    Makefile.
    - \b GCC/flash.ld:    Linker script.
  - Project configuration files using Keil IDE.
    - \b MDK-ARM/ble_proximity.uvprojx:
                             uVision5 project file. Contains the project
                             structure in XML format.
    - \b MDK-ARM/ble_proximity.uvoptx:
                             uVision5 project options. Contains the settings
                             for the debugger, trace configuration,
                             breakpoints, currently open files, etc.
    - \b MDK-ARM/flash.sct:  Linker script.
  - Project configuration files using IAR.
    - \b EWARM/ble_proximity.ewd:  IAR project options. Contains the settings
                                   for the debugger.
    - \b EWARM/ble_proximity.ewp:  IAR project file. Contains the project
                                   structure in XML format.
    - \b EWARM/ble_proximity.eww:  IAR workspace file. Contains project
                                   information.
    - \b EWARM/flash.icf:          Linker script.

@par Run the example
  - Build the sample project with the command "./build.sh mt2523_hdk
    ble_proximity" from the SDK root folder on GCC, or build the project
    ble_proximity.uvprojx from the MDK-ARM folder on Keil, or build the
    project ble_proximity.eww from the EWARM folder on IAR, then download the
    binary file to LinkIt 2523 HDK.
  - To run the application.
    - Install an app to support Bluetooth LE communication on a peer device,
      such "LightBlue" on iPhone.
    - Launch the app on the peer device and establish connection with LinkIt
      2523 HDK.
    - Write an alert level value from the app page, then LinkIt 2523 HDK will
      log this value.
*/
/**
 * @}
 * @}
 * @}
 */

