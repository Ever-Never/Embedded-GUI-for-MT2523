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
 * @addtogroup mt2523_hdk_apps_fota_download_manager fota_download_manager
 * @{


  @par Overview
  - Example project description
    - This example demonstrates the basic workflow of full binary FOTA
      update, including how to use the interface of Bluetooth notification
      service, triggering FOTA update process, retrieving FOTA update result.
      It also contains an example demonstrating GNSS firmware update.
  - Features of the application
    - This example project demonstrates how to use Bluetooth notification
      service to receive FOTA package and trigger update on the target board.

  @par Hardware and software environment
  - Supported platform
    - LinkIt 2523 HDK.
    - Android smart phone(Android OS version 4.0+)
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

  @par Directory contents
  - Source and header files
    - \b src/main.c:            Main program
    - \b src/system_mt2523.c:   MT2523x's system clock configuration file
    - \b src/fota_main.c:       Download manager main program, connect with
                                smart device application and exchange data
                                through btnotify service.
    - \b src/ept_gpio_var.c:    GPIO pin assignment with variable names.
    - \b src/ept_eint_var.c:    EINT assignment file with variable names.
    - \b inc/task_def.h:        Define the task stack size, queue length,
                                project name, and priority for the application
                                to create tasks.
    - \b inc/hal_feature_config.h:
                                MT2523x's feature configuration file.
    - \b inc/memory_map.h:      MT2523x's memory layout symbol file.
    - \b inc/ept_eint_drv.h:    EINT basic configuration header file.
    - \b inc/ept_gpio_drv.h:    GPIO pin assignment and basic configuration
                                file.
    - \b inc/ept_keypad_drv.h:  Keypad basic configuration header file.
    - \b GCC/startup_mt2523.s:  MT2523x's startup file for GCC.
    - \b GCC/syscalls.c:        MT2523x's syscalls for GCC.
    - \b MDK-ARM/startup_mt2523.s:
                                MT2523x's startup file for Keil.
    - \b src/gnss_src:          GNSS firmware update program, handshake with
                                GNSS and burn new firmware after downloading
                                finished.
  - Project configuration files using GCC.
    - \b GCC/feature.mk:  Feature configuration.
    - \b GCC/Makefile:    Makefile.
    - \b GCC/flash.ld:    Linker script.
  - Project configuration files using Keil.
    - \b MDK-ARM/fota_download_manager.uvprojx:
                             uVision5 project File. Contains the project
                             structure in XML format.
    - \b MDK-ARM/fota_download_manager.uvoptx:
                             uVision5 project options. Contains the settings
                             for the debugger, trace configuration,
                             breakpoints, currently open files, etc.
    - \b MDK-ARM/flash.sct:  Linker script.

  @par Run the example
  - Build the example project with the command, "./build.sh mt2523_hdk
    fota_download_manager bl" under the SDK root folder and download the
    binary file to LinkIt 2523 development board.
  - Install the application named "smartdevice.apk" on Android smartphone,
    and launch the application.
  - Power on LinkIt 2523 HDK.
  - Click "scan" item to start Bluetooth scanning in the application, find
    LinkIt 2523 device named "fota_download" in scanned list, and then click
    this name.
  - Wait for connection established between the smartphone and LinkIt 2523
    device, then click "Firmware Update(FBIN)" item in drop-down menu
    appeared at the top-right corner of screen.
  - Select the FOTA package file from the smartphone file system, then the
    application will start to push FOTA package to LinkIt 2523 device and
    enter FOTA update process.
  - The update progress will be shown in trace window, device will reboot
    automatically after downloading done, reconnect with smart phone and
    reply update result to application.

  @par Configuration
  - For IAR project, the bootloader.bin has been inserted into project binary
    default. So if want to build the binary without bootloader, use should
    change some configuration in IDE.
    - Frist click "option" to enter setting, navigate to "Linker"
      - > "Input", and clean all setting data in each edit box in this tab.
    - Second, replace the content of flash.icf instead of the content of
      flash_without_BL.icf.
    - Compile using IAR IDE, then the fota_download_manager.bin is built with
      bootloader now.
 * @}
 * @}
 * @}
 **/
